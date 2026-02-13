/**
 * @file FMODBackend.cpp
 * @author rahul
 * @brief FMOD-only implementation directly connecting with API
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"

#include "FMODBackend.h"

// CI override OR FMOD disabled => compile as empty TU
#if defined(MTS_CI) || !defined(MTS_AUDIO_FMOD)

// FMOD is disabled (CI / non-FMOD builds).
// This translation unit intentionally compiles as empty.

#else

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <ranges>
#include <unordered_map>

namespace mts
{
    static float Clamp01(float v)
    {
        return std::max(0.f, std::min(1.f, v));
    }

    static bool FmodOk(FMOD_RESULT r)
    {
        return r == FMOD_OK;
    }

    // Helper method for FMOD conversion
    static FMOD_VECTOR ToFmod(const glm::vec3 v)
    {
        FMOD_VECTOR o;
        o.x = v.x;
        o.y = v.y;
        o.z = v.z;
        return o;
    }

    struct FMODBackend::Impl
    {
        FMOD::Studio::System* studio = nullptr;
        FMOD::System* core = nullptr;

        // Banks and basic caches
        std::unordered_map<std::string, FMOD::Studio::Bank*> banks;
        std::unordered_map<std::string, FMOD::Studio::EventDescription*> eventDescCache;
        std::unordered_map<std::string, FMOD::Studio::Bus*> busCache;
        // new container for bank bytes to use file mannager
        std::unordered_map<std::string, std::vector<std::uint8_t> > bankBytes;

        // Handle table
        uint32_t nextHandle = 0;
        std::unordered_map<uint32_t, FMOD::Studio::EventInstance*> instances;

        SpatialAttributes listener{};

        FMOD::Studio::EventDescription* GetEventDesc(const std::string& eventId)
        {
            if (const auto it = eventDescCache.find(eventId); it != eventDescCache.end())
                return it->second;

            FMOD::Studio::EventDescription* desc = nullptr;
            FMOD_RESULT r = studio->getEvent(eventId.c_str(), &desc);
            if (!FmodOk(r) || !desc)
                return nullptr;

            eventDescCache[eventId] = desc;
            return desc;
        }

        FMOD::Studio::Bus* GetBus(const std::string& busPath)
        {
            if (auto it = busCache.find(busPath); it != busCache.end())
                return it->second;

            FMOD::Studio::Bus* bus = nullptr;
            FMOD_RESULT r = studio->getBus(busPath.c_str(), &bus);
            if (!FmodOk(r) || !bus)
                return nullptr;

            busCache[busPath] = bus;
            return bus;
        }

        AudioHandle MakeHandle(FMOD::Studio::EventInstance* instance)
        {
            if (!instance)
                return {};

            uint32_t h = ++nextHandle;
            instances[h] = instance;
            return AudioHandle{h};
        }

        FMOD::Studio::EventInstance* GetInstance(AudioHandle h)
        {
            if (!h)
                return nullptr;

            auto it = instances.find(h.value);
            return (it != instances.end()) ? it->second : nullptr;
        }

        void RemoveInstance(AudioHandle h)
        {
            if (!h)
                return;
            instances.erase(h.value);
        }
    };

    FMODBackend::FMODBackend() : m(new Impl)
    {
    }

    FMODBackend::~FMODBackend()
    {
        Shutdown();
        delete m;
        m = nullptr;
    }

    //new function that communicates with file manager
    bool FMODBackend::LoadBankBytes(const std::string& bankKey,
                                    const std::vector<std::uint8_t>& bytes)
    {
        if (!m || !m->studio)
            return false;

        if (bytes.empty())
            return false;

        // FMOD wants "int length"
        if (bytes.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
            return false;

        const char* mem = reinterpret_cast<const char*>(bytes.data());
        const int len = static_cast<int>(bytes.size());

        if (m->banks.contains(bankKey))
            return true;

        FMOD::Studio::Bank* bank = nullptr;

        FMOD_RESULT r = m->studio->loadBankMemory(mem,len,FMOD_STUDIO_LOAD_MEMORY,  FMOD_STUDIO_LOAD_BANK_NORMAL,&bank);

        if (r != FMOD_OK || !bank)
        {
            LOG_ERROR("[FMODBackend] loadBankMemory failed. key='{}' size={} err='{}'", bankKey,bytes.size(), FMOD_ErrorString(r));
            return false;
        }

        m->banks[bankKey] = bank;
        bank->loadSampleData();
        return true;
    }
    bool FMODBackend::Init(const AudioConfig& config)
    {
        if (m->studio)
            return true;

        FMOD_RESULT r = FMOD::Studio::System::create(&m->studio);
        if (!FmodOk(r) || !m->studio)
            return false;

        // Studio init flags
        FMOD_STUDIO_INITFLAGS studioFlags = FMOD_STUDIO_INIT_NORMAL;
        if (config.liveUpdate)
            studioFlags |= FMOD_STUDIO_INIT_LIVEUPDATE;

        // Core init flags
        FMOD_INITFLAGS coreFlags = FMOD_INIT_NORMAL;

        r = m->studio->initialize(config.maxVirtualVoices, studioFlags, coreFlags, nullptr);
        if (!FmodOk(r))
            return false;

        // Note: Real voice count can also be set here. FMOD "real voices" is more nuanced; leaving
        // basic init is fine initially.
        return true;
    }

    void FMODBackend::Shutdown()
    {
        if (!m->studio)
            return;

        // Release all instances
        for (auto& instance : m->instances | std::views::values)
        {
            if (instance)
            {
                instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
                instance->release();
            }
        }
        m->instances.clear();

        // Unload banks

        for (auto& bank : m->banks | std::views::values)
        {
            if (bank)
                bank->unload();
        }
        m->banks.clear();

        m->busCache.clear();
        m->eventDescCache.clear();

        m->studio->release();
        m->studio = nullptr;
        m->core = nullptr;
    }

    void FMODBackend::Update(float dt)
    {
        if (m->studio)
            m->studio->update();

        // Clean up work to remove stopped instances to avoid handle table growth.
        std::vector<uint32_t> toRemove;
        toRemove.reserve(32);

        for (auto& [h, instance] : m->instances)
        {
            if (!instance)
            {
                toRemove.push_back(h);
                continue;
            }
            FMOD_STUDIO_PLAYBACK_STATE state;
            if (FmodOk(instance->getPlaybackState(&state)))
            {
                if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
                {
                    instance->release();
                    toRemove.push_back(h);
                }
            }
        }
        for (auto h : toRemove)
            m->instances.erase(h);
    }

    bool FMODBackend::LoadBank(const std::string& bankPath)
    {
        if (!m->studio)
            return false;
        if (m->banks.contains(bankPath))
            return true;

        FMOD::Studio::Bank* bank = nullptr;
        FMOD_RESULT r =
            m->studio->loadBankFile(bankPath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
        if (!FmodOk(r) || !bank)
            return false;

        m->banks[bankPath] = bank;

        bank->loadSampleData();
        return true;
    }
    bool FMODBackend::IsPlaying(AudioHandle handle) const
    {
        auto it = m->instances.find(handle.value);
        if (it == m->instances.end() || !it->second)
            return false;  // no instance → definitely not playing

        FMOD_STUDIO_PLAYBACK_STATE state;
        if (!FmodOk(it->second->getPlaybackState(&state)))
            return false;

        return state != FMOD_STUDIO_PLAYBACK_STOPPED;
    }
    bool FMODBackend::UnloadBank(const std::string& bankPath)
    {
        auto it = m->banks.find(bankPath);
        if (it == m->banks.end())
            return true;

        FMOD::Studio::Bank* bank = it->second;
        if (bank)
        {
            bank->unloadSampleData();
            bank->unload();
        }
        m->banks.erase(it);
        m->bankBytes.erase(bankPath);
        // Caches may also at this point contain invalid pointers.
        m->eventDescCache.clear();
        m->busCache.clear();

        return true;
    }

    AudioHandle FMODBackend::CreateInstance(const AudioEventId& eventId)
    {
        if (!m->studio)
            return {};

        const FMOD::Studio::EventDescription* desc = m->GetEventDesc(eventId);
        if (!desc)
            return {};

        FMOD::Studio::EventInstance* instance = nullptr;
        FMOD_RESULT r = desc->createInstance(&instance);
        if (!FmodOk(r) || !instance)
            return {};

        return m->MakeHandle(instance);
    }

    void FMODBackend::Start(AudioHandle h)
    {
        if (auto* instance = m->GetInstance(h))
            instance->start();
    }

    void FMODBackend::Stop(AudioHandle h, StopMode mode)
    {
        if (auto* instance = m->GetInstance(h))
        {
            // Can be handled further if we decide to add more types of stop modes.
            auto fmodMode = (mode == StopMode::Immediate) ? FMOD_STUDIO_STOP_IMMEDIATE
                                                          : FMOD_STUDIO_STOP_ALLOWFADEOUT;
            instance->stop(fmodMode);
        }
    }

    void FMODBackend::Release(AudioHandle h)
    {
        if (auto* instance = m->GetInstance(h))
        {
            instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
            instance->release();
        }
        m->RemoveInstance(h);
    }

    void FMODBackend::SetListenerAttributes(int listenerIndex, const SpatialAttributes& a)
    {
        if (!m->studio)
            return;

        FMOD_3D_ATTRIBUTES att{};
        att.position = ToFmod(a.pos);
        att.velocity = ToFmod(a.vel);
        att.forward = ToFmod(a.forward);
        att.up = ToFmod(a.up);

        // Studio API call
        m->studio->setListenerAttributes(listenerIndex, &att);
    }

    void FMODBackend::SetInstanceAttributes(AudioHandle h, const SpatialAttributes& a)
    {
        auto* instance = m->GetInstance(h);
        if (!instance)
            return;

        FMOD_3D_ATTRIBUTES att{};
        att.position = ToFmod(a.pos);
        att.velocity = ToFmod(a.vel);
        att.forward = ToFmod(a.forward);
        att.up = ToFmod(a.up);

        instance->set3DAttributes(&att);
    }

    void FMODBackend::SetParameter(AudioHandle h, const std::string& name, float value)
    {
        if (auto* instance = m->GetInstance(h))
            instance->setParameterByName(name.c_str(), value);
    }

    void FMODBackend::SetGlobalParameter(const std::string& name, float value)
    {
        if (!m->studio)
            return;

        m->studio->setParameterByName(name.c_str(), value);
    }

    void FMODBackend::SetBusVolume(const std::string& busPath, float volume01)
    {
        if (!m->studio)
            return;

        if (auto* bus = m->GetBus(busPath))
            bus->setVolume(Clamp01(volume01));
    }

    void FMODBackend::SetBusPaused(const std::string& busPath, bool paused)
    {
        if (!m->studio)
            return;

        if (auto* bus = m->GetBus(busPath))
            bus->setPaused(paused);
    }

    AudioHandle FMODBackend::StartSnapshot(const std::string& snapshotEventId)
    {
        // In FMOD, snapshots are also events, so we need to start them like any event instance.
        AudioHandle h = CreateInstance(snapshotEventId);
        if (h)
            Start(h);
        return h;
    }

}  // namespace mts

#endif  // MTS_AUDIO_FMOD
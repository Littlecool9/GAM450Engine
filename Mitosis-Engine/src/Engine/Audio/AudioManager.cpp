/**
 * @file AudioManager.cpp
 * @author rahul
 * @brief Engine-facing audio service built on IAudioBackend
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AudioManager.h"

#include "Engine/Serialization/FileManager.h"

namespace mts
{
    AudioManager* AudioManager::sManager = nullptr;

    AudioManager::AudioManager()
    {
        // Default bus paths (FMOD-style)
        mBusPaths[AudioCategory::Master] = "bus:/";
        mBusPaths[AudioCategory::Music] = "bus:/Music";
        mBusPaths[AudioCategory::SFX] = "bus:/SFX";
        mBusPaths[AudioCategory::UI] = "bus:/UI";
        mBusPaths[AudioCategory::Ambience] = "bus:/Ambience";

        mCategoryVolumes[AudioCategory::Master]   = 1.0f;
        mCategoryVolumes[AudioCategory::Music]    = 1.0f;
        mCategoryVolumes[AudioCategory::SFX]      = 1.0f;
        mCategoryVolumes[AudioCategory::UI]       = 1.0f;
        mCategoryVolumes[AudioCategory::Ambience] = 1.0f;
    }

    AudioManager::~AudioManager()
    {
        Shutdown();
    }

    bool AudioManager::Init(std::unique_ptr<IAudioBackend> backend, const AudioConfig& config)
    {
        if (mInitialized)
        {
            LOG_WARN("Init called more than once. Ignoring.");
            return true;
        }

        if (!backend)
        {
            LOG_CRITICAL("Init failed: backend is null.");
            return false;
        }

        mBackend = std::move(backend);
        mConfig = config;

        if (!mBackend->Init(mConfig))
        {
            LOG_CRITICAL("Backend initialization failed.");
            mBackend.reset();
            return false;
        }

        mInitialized = true;
        LOG_INFO("Audio system initialized successfully.");
        return true;
    }

    void AudioManager::Shutdown()
    {
        if (!mInitialized)
            return;

        LOG_INFO("Shutting down audio system.");

        StopMusic(StopMode::Immediate);

        // Unload all loaded groups
        std::vector groups(mLoadedGroups.begin(), mLoadedGroups.end());
        for (auto g : groups)
            UnloadBankGroup(g);
        mLoadedGroups.clear();

        mBackend->Shutdown();
        mBackend.reset();
        mInitialized = false;
    }

    void AudioManager::Update(float dt)
    {
        if (!EnsureInitOrWarn("Update"))
            return;

        // Update listener attributes
        SpatialAttributes attributes = Make2DAttributes(mListenerPos, mListenerVel);
        mBackend->SetListenerAttributes(0, attributes);

        // Letting the backend tick FMOD and cleanup stopped handles
        mBackend->Update(dt);

        UpdateRhythm(dt);
    }

    void AudioManager::SetListener2D(glm::vec2 pos, glm::vec2 vel)
    {
        mListenerPos = pos;
        mListenerVel = vel;
    }

    void AudioManager::SetInstance2D(AudioHandle h, glm::vec2 pos, glm::vec2 vel) const
    {
        if (!EnsureInitOrWarn(__func__))
            return;

        if (!h)  // invalid handle
            return;

        SpatialAttributes attrs = Make2DAttributes(pos, vel);
        mBackend->SetInstanceAttributes(h, attrs);
    }

    void AudioManager::AddBankToGroup(BankGroup group, const std::string& bankPath)
    {
        mBankGroups[group].push_back(bankPath);
    }

    bool AudioManager::LoadBankGroup(BankGroup group)
    {
        if (!EnsureInitOrWarn("LoadBankGroup"))
            return false;

        if (mLoadedGroups.contains(group))
        {
            LOG_WARN("Bank group already loaded.");
            return true;
        }

        auto it = mBankGroups.find(group);
        if (it == mBankGroups.end())
        {
            LOG_ERROR("Bank group has no banks assigned.");
            return false;
        }

        bool ok = true;
        // old code
        // for (const auto& path : it->second)
        //     ok = mBackend->LoadBank(path) && ok;
        //
        for (const std::string& logicalPath : it->second)
        {
            std::vector<std::uint8_t> bytes;

            // using file manager for io
            if (!FileManager::ReadAllBytes(logicalPath, bytes))
            {
                LOG_ERROR("[AudioManager] Failed to read bank '{}' (resolved='{}')",
                          logicalPath,
                          FileManager::Resolve(logicalPath));
                ok = false;
                continue;
            }

            // Backend loads from memory
            if (!mBackend->LoadBankBytes(logicalPath, bytes))
            {
                LOG_ERROR("[AudioManager] Backend failed to load bank '{}' (size={})",
                          logicalPath,
                          bytes.size());
                ok = false;
            }
        }
        if (ok)
        {
            LOG_INFO("Bank group loaded successfully.");
            mLoadedGroups.insert(group);
        }
        else
        {
            LOG_ERROR("Failed to load one or more banks in group.");
        }

        return ok;
    }

    bool AudioManager::LoadBankFile(const std::string& bankPath)
    {
        if (!EnsureInitOrWarn("LoadBankFile"))
            return false;

        if (!mBackend->LoadBank(bankPath))
        {
            LOG_ERROR("[AudioManager] LoadBankFile failed for '{}'.", bankPath);
            return false;
        }

        LOG_INFO("[AudioManager] LoadBankFile succeeded for '{}'.", bankPath);
        return true;
    }

    bool AudioManager::UnloadBankFile(const std::string& bankPath)
    {
        if (!EnsureInitOrWarn("UnloadBankFile"))
            return false;

        return mBackend->UnloadBank(bankPath);
    }

    bool AudioManager::UnloadBankGroup(BankGroup group)
    {
        if (!EnsureInitOrWarn("UnloadBankGroup"))
            return false;

        if (!mLoadedGroups.contains(group))
            return true;

        auto it = mBankGroups.find(group);
        if (it == mBankGroups.end())
            return true;

        bool ok = true;
        for (const auto& path : it->second)
            ok = mBackend->UnloadBank(path) && ok;

        mLoadedGroups.erase(group);
        return ok;
    }

    AudioHandle AudioManager::Play(const AudioEventId& eventId) const
    {
        if (!EnsureInitOrWarn("Play"))
            return {};

        const AudioHandle h = mBackend->CreateInstance(eventId);
        if (!h)
        {
            LOG_ERROR("Failed to create audio instance for event '{}'.", eventId);
            return {};
        }

        mBackend->Start(h);
        return h;
    }

    AudioHandle AudioManager::Play2D(const AudioEventId& eventId, glm::vec2 pos) const
    {
        if (!EnsureInitOrWarn("Play2D"))
            return {};

        const AudioHandle h = mBackend->CreateInstance(eventId);
        if (!h)
        {
            LOG_ERROR("Failed to create audio instance for event '{}'.", eventId);
            return {};
        }

        // Setting the spatial before starting
        mBackend->SetInstanceAttributes(h, Make2DAttributes(pos));
        mBackend->Start(h);

        return h;
    }

    void AudioManager::Stop(AudioHandle h, StopMode mode) const
    {
        if (!EnsureInitOrWarn("Stop"))
            return;

        if (h)
            mBackend->Stop(h, mode);
    }

    void AudioManager::Release(AudioHandle h) const
    {
        if (!EnsureInitOrWarn("Release"))
            return;

        if (h)
            mBackend->Release(h);
    }

    void AudioManager::PlayOneShot(const AudioEventId& eventId) const
    {
        AudioHandle h = Play(eventId);
        if (h)
        {
            mBackend->Release(h);
        }
    }

    void AudioManager::PlayOneShot2D(const AudioEventId& eventId, glm::vec2 pos) const
    {
        AudioHandle h = Play2D(eventId, pos);
        if (h)
        {
            mBackend->Release(h);
        }
    }

    void AudioManager::SetParameter(AudioHandle h, const std::string& name, float value) const
    {
        if (!EnsureInitOrWarn("SetParameter"))
            return;

        if (h)
            mBackend->SetParameter(h, name, value);
    }

    void AudioManager::SetGlobalParameter(const std::string& name, float value) const
    {
        if (!EnsureInitOrWarn("SetGlobalParameter"))
            return;

        mBackend->SetGlobalParameter(name, value);
    }

    const char* AudioManager::GetBusPath(AudioCategory category) const
    {
        auto it = mBusPaths.find(category);
        if (it == mBusPaths.end())
            return "bus:/";

        return it->second.c_str();
    }

    void AudioManager::SetCategoryVolume(AudioCategory category, float volume01) const
    {
        if (!EnsureInitOrWarn("SetCategoryVolume"))
            return;

        mCategoryVolumes[category] = volume01;
        mBackend->SetBusVolume(GetBusPath(category), volume01);
    }

    float AudioManager::GetCategoryVolume(AudioCategory category) const
    {
        auto it = mCategoryVolumes.find(category);
        return (it != mCategoryVolumes.end()) ? it->second : 1.0f;
    }

    void AudioManager::SetCategoryPaused(AudioCategory category, bool paused) const
    {
        if (!EnsureInitOrWarn("SetCategoryPaused"))
            return;

        mBackend->SetBusPaused(GetBusPath(category), paused);
    }

    void AudioManager::PauseAll() const
    {
        if (!EnsureInitOrWarn("PauseAll"))
            return;

        mBackend->SetBusPaused(GetBusPath(AudioCategory::Master), true);
    }

    void AudioManager::ResumeAll() const
    {
        if (!EnsureInitOrWarn("ResumeAll"))
            return;

        mBackend->SetBusPaused(GetBusPath(AudioCategory::Master), false);
    }

    AudioHandle AudioManager::StartSnapshot(const std::string& snapshotEventId) const
    {
        if (!EnsureInitOrWarn("StartSnapshot"))
            return {};

        return mBackend->StartSnapshot(snapshotEventId);
    }

    void AudioManager::StopSnapshot(AudioHandle snapshotHandle, StopMode mode) const
    {
        if (!EnsureInitOrWarn("StopSnapshot"))
            return;

        if (!snapshotHandle)
            return;

        mBackend->Stop(snapshotHandle, mode);
        mBackend->Release(snapshotHandle);
    }

    bool AudioManager::StartMusic(const MusicTrack& track)
    {
        if (!EnsureInitOrWarn("StartMusic"))
            return false;

        // Existing music needs to be stopped
        StopMusic(StopMode::Immediate);

        mCurrentTrack = track;
        mSongTime = 0.f;
        mBeatIndex = 0;
        mBeatPhase = 0.f;

        mMusicHandle = Play(track.eventId);
        if (!mMusicHandle)
        {
            LOG_ERROR("Failed to start music track '{}'.", track.trackId);
            mMusicPlaying = false;
            return false;
        }

        mMusicPlaying = true;
        LOG_INFO("Music track '{}' started.", track.trackId);
        return true;
    }

    void AudioManager::StopMusic(StopMode mode)
    {
        if (!mMusicPlaying || !mMusicHandle)
            return;

        mBackend->Stop(mMusicHandle, mode);
        mBackend->Release(mMusicHandle);
        mMusicHandle = {};
        mMusicPlaying = false;
    }
    bool AudioManager::IsPlaying(AudioHandle h) const
    {
        if (!EnsureInitOrWarn("IsPlaying"))
            return false;
        return mBackend->IsPlaying(h);
    }

    void AudioManager::UpdateRhythm(float dt)
    {
        if (!mMusicPlaying)
            return;

        mSongTime += dt;

        // Beat phase: (songTime - offset) / beatPeriod
        const float beatPeriod = (mCurrentTrack.bpm > 0.0f) ? (60.0f / mCurrentTrack.bpm) : 0.5f;
        const float t = mSongTime - mCurrentTrack.offsetSec;

        if (beatPeriod <= 0.f)
            return;

        // For negative t (before beat 0), clamp to beat 0 phase
        const float beatFloat = (t <= 0.f) ? 0.f : (t / beatPeriod);
        const int beatIndex = static_cast<int>(std::floor(beatFloat));
        const float phase = beatFloat - static_cast<float>(beatIndex);

        mBeatIndex = beatIndex;
        mBeatPhase = phase;

        mBackend->SetGlobalParameter("BeatPhase", mBeatPhase);
    }

    bool AudioManager::IsInBeatWindow(float earlySec, float lateSec) const
    {
        if (!mMusicPlaying || mCurrentTrack.bpm <= 0.f)
            return false;

        const float beatPeriod = 60.f / mCurrentTrack.bpm;

        // Time since last beat
        const float t = mSongTime - mCurrentTrack.offsetSec;
        if (t < 0.f)
            return false;

        const float beatFloat = t / beatPeriod;
        const float phase = beatFloat - std::floor(beatFloat);
        const float timeSinceLastBeat = phase * beatPeriod;

        // On-beat window around beat start
        return (timeSinceLastBeat <= lateSec) || (timeSinceLastBeat >= beatPeriod - earlySec);
    }

    bool AudioManager::EnsureInitOrWarn(const char* fn) const
    {
        if (!mInitialized || !mBackend)
        {
            LOG_WARN("{} called before AudioManager::Init(). Call ignored.", fn);
            return false;
        }
        return true;
    }
}  // namespace mts
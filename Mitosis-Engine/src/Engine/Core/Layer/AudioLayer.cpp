/**
 * @file AudioLayer.cpp
 * @author sumin.park
 * @brief  Layer for audio system
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AudioLayer.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/Conductor/ConductorSystem.h"

#ifdef MTS_CI
#define MTS_AUDIO_USE_FMOD 0
#else
#define MTS_AUDIO_USE_FMOD 1
#endif

#if MTS_AUDIO_USE_FMOD
#include "Engine/Audio/Backends/FMODBackend.h"
#else
#include "Audio/Backends/NullAudioBackend.h"
#endif
namespace mts
{
    AudioLayer::AudioLayer() : Layer("AudioLayer")
    {
    }

    void AudioLayer::OnAttach()
    {
        mAudio = std::make_unique<AudioManager>();

        AudioManager::SetManager(mAudio.get());

        AudioConfig audioConfig{};
        audioConfig.liveUpdate = true;  // Dev only
        audioConfig.maxVirtualVoices = 1024;
        audioConfig.maxRealVoices = 64;

        // Create backend
        std::unique_ptr<IAudioBackend> backend;

#if defined(MTS_AUDIO_FMOD) && !defined(MTS_CI)
        backend = std::make_unique<FMODBackend>();
#else
        backend = std::make_unique<NullAudioBackend>();
#endif

        if (!mAudio->Init(std::move(backend), audioConfig))
        {
            LOG_CRITICAL("AudioManager Init failed.");
        }
        else
        {
            LOG_INFO("AudioManager Init OK.");
        }
        mAudio->AddBankToGroup(BankGroup::Core, "Assets/Audio/MausoleumMash/Master.bank");
        mAudio->AddBankToGroup(BankGroup::Core, "Assets/Audio/MausoleumMash/Master.strings.bank");
        mAudio->AddBankToGroup(BankGroup::Music, "Assets/Audio/MausoleumMash/Music.bank");
        mAudio->LoadBankGroup(BankGroup::Core);
        mAudio->LoadBankGroup(BankGroup::Music);

        mConductor = std::make_unique<ConductorSystem>();
        ConductorSystem::SetInstance(mConductor.get());
        mConductor->Init();

        Layer::OnAttach();
    }

    void AudioLayer::OnDetach()
    {
        mAudio->Shutdown();
        mConductor->Shutdown();
        Layer::OnDetach();
    }

    void AudioLayer::OnLateUpdate(float dt)
    {
        mAudio->Update(dt);
        mConductor->Update(dt);
    }
}  // namespace mts
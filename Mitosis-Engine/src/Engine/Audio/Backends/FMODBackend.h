/**
 * @file FMODBackend.h
 * @author rahul
 * @brief Child class inheriting definitions from IAudioBackend specifically for FMOD API
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#if !defined(MTS_AUDIO_FMOD) && !defined(MTS_CI)
#error FMODBackend included but MTS_AUDIO_FMOD is not enabled. Use NullAudioBackend.
#endif

#include "Engine/Audio/IAudioBackend.h"

namespace mts
{
    class FMODBackend final : public IAudioBackend
    {
       public:
        FMODBackend();
        ~FMODBackend() override;

        bool Init(const AudioConfig& config) override;
        void Shutdown() override;
        void Update(float dt) override;

        bool LoadBank(const std::string& bankPath) override;
        // using this instead of LoadBank for filemanager support 
        bool LoadBankBytes(const std::string& bankKey, const std::vector<std::uint8_t>& bytes) override;

        bool UnloadBank(const std::string& bankPath) override;
        bool IsPlaying(AudioHandle handle) const override;
        
        AudioHandle CreateInstance(const AudioEventId& eventId) override;
        void Start(AudioHandle h) override;
        void Stop(AudioHandle h, StopMode mode) override;
        void Release(AudioHandle h) override;

        void SetListenerAttributes(int listenerIndex, const SpatialAttributes& a) override;
        void SetInstanceAttributes(AudioHandle h, const SpatialAttributes& a) override;

        void SetParameter(AudioHandle h, const std::string& name, float value) override;
        void SetGlobalParameter(const std::string& name, float value) override;

        void SetBusVolume(const std::string& busPath, float volume01) override;
        void SetBusPaused(const std::string& busPath, bool paused) override;

        AudioHandle StartSnapshot(const std::string& snapshotEventId) override;

       private:
        struct Impl;
        Impl* m = nullptr;
    };

}  // namespace mts
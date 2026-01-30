/**
 * @file FMODBackend.h
 * @author rahul
 * @brief Child class inheriting definitions from IAudioBackend specifically for FMOD API
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Audio/IAudioBackend.h"

namespace mts::audio
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
        bool UnloadBank(const std::string& bankPath) override;

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

}  // namespace mts::audio
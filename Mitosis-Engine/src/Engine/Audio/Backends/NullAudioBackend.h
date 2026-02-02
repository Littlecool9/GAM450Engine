/**
 * @file NullAudioBackend.h
 * @author rahul
 * @brief No-op audio backend used when nothing is available (CI / server builds)
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Audio/IAudioBackend.h"

namespace mts
{
    class NullAudioBackend final : public IAudioBackend
    {
       public:
        NullAudioBackend() = default;
        ~NullAudioBackend() override = default;

        // Lifecycle
        bool Init(const AudioConfig& config) override;
        void Shutdown() override;
        void Update(float dt) override;

        // Content
        bool LoadBank(const std::string& bankPath) override;
        bool UnloadBank(const std::string& bankPath) override;

        // Playback
        AudioHandle CreateInstance(const AudioEventId& eventId) override;
        void Start(AudioHandle h) override;
        void Stop(AudioHandle h, StopMode mode) override;
        void Release(AudioHandle h) override;

        // Spatial
        void SetListenerAttributes(int listenerIndex, const SpatialAttributes& a) override;
        void SetInstanceAttributes(AudioHandle h, const SpatialAttributes& a) override;

        // Parameters
        void SetParameter(AudioHandle h, const std::string& name, float value) override;
        void SetGlobalParameter(const std::string& name, float value) override;

        // Mixing
        void SetBusVolume(const std::string& busPath, float volume01) override;
        void SetBusPaused(const std::string& busPath, bool paused) override;

        // Snapshots
        AudioHandle StartSnapshot(const std::string& snapshotEventId) override;
    };

}  // namespace mts
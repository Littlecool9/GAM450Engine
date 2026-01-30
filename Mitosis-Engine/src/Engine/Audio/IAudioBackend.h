/**
 * @file IAudioBackend.h
 * @author rahul
 * @brief Interface for generic backend connection to Audio library
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "AudioTypes.h"

namespace mts::audio
{
    /**
     * Banks = what exists in memory
     * Events = what you play
     * Buses = how sounds are grouped & mixed
     * Snapshots = temporary mix states
     */
    class IAudioBackend
    {
       public:
        virtual ~IAudioBackend() = default;

        virtual bool Init(const AudioConfig& config) = 0;
        virtual void Shutdown() = 0;
        virtual void Update(float dt) = 0;

        // Content
        virtual bool LoadBank(const std::string& bankPath) = 0;
        virtual bool UnloadBank(const std::string& bankPath) = 0;

        // Playback
        virtual AudioHandle CreateInstance(const AudioEventId& eventId) = 0;
        virtual void Start(AudioHandle h) = 0;
        virtual void Stop(AudioHandle h, StopMode mode) = 0;
        virtual void Release(AudioHandle h) = 0;  // safe to call multiple times

        // Spatial (will work for 2D and 3D)
        virtual void SetListenerAttributes(int listenerIndex, const SpatialAttributes& a) = 0;
        virtual void SetInstanceAttributes(AudioHandle h, const SpatialAttributes& a) = 0;

        // Parameters
        virtual void SetParameter(AudioHandle h, const std::string& name, float value) = 0;
        virtual void SetGlobalParameter(const std::string& name, float value) = 0;

        // Mixing
        // TODO: Curently done using buses by path but needs to be friendly name mapping later
        virtual void SetBusVolume(const std::string& busPath, float volume01) = 0;
        virtual void SetBusPaused(const std::string& busPath, bool paused) = 0;

        // Snapshots
        virtual AudioHandle StartSnapshot(
            const std::string& snapshotEventId) = 0;  // returns handle to control/stop
    };

}  // namespace mts::audio

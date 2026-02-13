/**
 * @file NullAudioBackend.cpp
 * @author rahul
 * @brief No-op audio backend used when nothing is available (CI / server builds)
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "NullAudioBackend.h"

namespace mts
{
    bool NullAudioBackend::Init(const AudioConfig&)
    {
        LOG_WARN("NullAudioBackend initialized (audio disabled).");
        return true;
    }

    void NullAudioBackend::Shutdown()
    {
        LOG_INFO("NullAudioBackend shutdown.");
    }

    void NullAudioBackend::Update(float)
    {
        // no-op
    }

    bool NullAudioBackend::LoadBank(const std::string&)
    {
        return true;
    }

    bool NullAudioBackend::UnloadBank(const std::string&)
    {
        return true;
    }

    bool NullAudioBackend::LoadBankBytes(const std::string& bankKey,
                                         const std::vector<std::uint8_t>& bytes)
    {
        return true;
    }

    AudioHandle NullAudioBackend::CreateInstance(const AudioEventId&)
    {
        return {};  // invalid handle (silently ignored by AudioManager)
    }

    void NullAudioBackend::Start(AudioHandle)
    {
        // no-op
    }

    void NullAudioBackend::Stop(AudioHandle, StopMode)
    {
        // no-op
    }

    void NullAudioBackend::Release(AudioHandle)
    {
        // no-op (safe to call multiple times)
    }

    bool NullAudioBackend::IsPlaying(AudioHandle handle) const
    {
        return true;
    }

    void NullAudioBackend::SetListenerAttributes(int, const SpatialAttributes&)
    {
        // no-op
    }

    void NullAudioBackend::SetInstanceAttributes(AudioHandle, const SpatialAttributes&)
    {
        // no-op
    }

    void NullAudioBackend::SetParameter(AudioHandle, const std::string&, float)
    {
        // no-op
    }

    void NullAudioBackend::SetGlobalParameter(const std::string&, float)
    {
        // no-op
    }

    void NullAudioBackend::SetBusVolume(const std::string&, float)
    {
        // no-op
    }

    void NullAudioBackend::SetBusPaused(const std::string&, bool)
    {
        // no-op
    }

    AudioHandle NullAudioBackend::StartSnapshot(const std::string&)
    {
        return {};  // snapshots silently ignored
    }
}  // namespace mts
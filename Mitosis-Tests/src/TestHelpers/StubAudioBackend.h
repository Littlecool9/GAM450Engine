#pragma once
#include "Engine/Audio/IAudioBackend.h"

namespace mts
{
    class StubAudioBackend final : public IAudioBackend
    {
    public:
        bool Init(const AudioConfig&) override                                  { return true; }
        void Shutdown() override                                                {}
        void Update(float) override                                             {}
        bool LoadBank(const std::string&) override                              { return true; }
        bool UnloadBank(const std::string&) override                            { return true; }
        bool LoadBankBytes(const std::string&,
                           const std::vector<uint8_t>&) override                { return true; }
        AudioHandle CreateInstance(const AudioEventId&) override
        {
            return AudioHandle{1u};  // non-zero = valid handle
        }
        void        Start(AudioHandle) override                                 {}
        void        Stop(AudioHandle, StopMode) override                        {}
        void        Release(AudioHandle) override                               {}
        bool        IsPlaying(AudioHandle) const override                       { return true; }
        void        SetListenerAttributes(int, const SpatialAttributes&) override {}
        void        SetInstanceAttributes(AudioHandle,
                                          const SpatialAttributes&) override    {}
        void        SetParameter(AudioHandle,
                                 const std::string&, float) override            {}
        void        SetGlobalParameter(const std::string&, float) override      {}
        void        SetBusVolume(const std::string&, float) override            {}
        void        SetBusPaused(const std::string&, bool) override             {}
        AudioHandle StartSnapshot(const std::string&) override                  { return {}; }
    };
}  // namespace mts

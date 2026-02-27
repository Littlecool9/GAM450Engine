/**
 * @file AudioEmitter.h
 * @author Dong Ta
 * @brief Component to allow a gameobject to emit audio
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */
#pragma once
#include "Engine/Audio/AudioTypes.h"
#include "Engine/EntitySystem/Component/Component.h"

namespace mts
{
    class AudioManager;
    class Transform;

    class AudioEmitter : public Component
    {
       public:
        AudioEventId eventId{};
        AudioHandle handle{};
        // Optional offset from the GameObject’s position
        glm::vec2 mPositionOffset{0.f, 0.f};
        float mStartVolume = 1.0f;
        float mStartPitch = 1.0f;
        float mMaxAudibleDistance = 900.0f;
        bool mPlayOnStart = false;   // auto play when component starts
        bool mSpatialized2D = true;  // false => non-spatial Play()
        bool mIsOneShot = false;     // auto Release after Stop, or just fire-and-forget
        bool isPlaying = false;

        AudioEmitter();
        AudioEmitter(GameObject* owner);
        ~AudioEmitter() override;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        // Manual control (from gameplay)
        void Play();
        void Stop(StopMode mode = StopMode::AllowFadeOut);
        void SetVolume(float value);
        void SetPitch(float value);
        bool IsPlaying() const;
        Component* Clone(GameObject* newOwner) const override;

       private:
        glm::vec2 mPrevPos{0.f, 0.f};
        glm::vec2 Get2DPosition() const;
        Transform* GetTransform() const;
        bool mFirstFrame = true;
    };
}  // namespace mts
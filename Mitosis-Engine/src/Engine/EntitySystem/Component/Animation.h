/**
 * @file Animation.h
 * @author Dong Ta
 * @brief  Animation component
 * @version 0.1
 * @date 2/7/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#pragma once
#include "Component.h"

namespace mts
{
    class Animation : public Component
    {
       public:
        Animation();
        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void AnimationPlay(int startFrame, int frameCount, float frameDuration, bool isLooping);
        bool AnimationIsDone() const;
        void AnimationStop();
        Component* Clone(GameObject* newOwner) const override;

       private:
        // The current frame being displayed.
        int mFrameIndex = 0;

        // The starting frame.
        int mStartFrame = 0;

        // The maximum number of frames in the sequence.
        int mFrameCount = 0;

        // The time remaining for the current frame.
        float mFrameDelay = 0.f;

        // The amount of time to display each successive frame.
        float mFrameDuration = 0.f;

        // True if the animation is running; false if the animation has stopped.
        bool mIsRunning = false;

        // True if the animation loops back to the beginning.
        bool mIsLooping = false;

        // True if the end of the animation has been reached, false otherwise.
        bool IsDone = false;

        friend class AnimationSystem;
    };

}  // namespace mts

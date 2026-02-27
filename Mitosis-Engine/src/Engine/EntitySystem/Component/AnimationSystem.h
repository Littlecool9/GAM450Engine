/**
 * @file AnimationSystem.h
 * @author Dong Ta
 * @brief  FSM for animation states
 * @version 0.1
 * @date 2/9/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once

#include "Component.h"

namespace mts
{
    class Mesh;
    class Shader;
    class Texture;
    class SpriteRenderer;
    class Animation;
    class AnimationSystem : public Component
    {
       public:
        // add more states here
        enum class States
        {
            Idle = 0,
            Walking,
            Jump,
            Falling,
            Dance
        };

        struct Info
        {
            Mesh* mMesh = nullptr;
            Shader* mShader = nullptr;
            Texture* mTexture = nullptr;

            bool animation = false;

            // The current frame being displayed.
            int frameIndex = 0;

            // The maximum number of frames in the sequence.
            int frameCount = 0;

            // The time remaining for the current frame.
            float frameDelay = 0.f;

            // The amount of time to display each successive frame.
            float frameDuration = 0.f;

            // True if the animation is running; false if the animation has stopped.
            bool isRunning = false;

            // True if the animation loops back to the beginning.
            bool isLooping = false;

            std::string meshPath;
            std::string shaderPath;
            std::string textPath;
        };

        std::unordered_map<States, Info> dictionary;
        AnimationSystem();
        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void ChangeState(States NewState);
        bool IsDone();
        States GetCurrentState() { return curStates; }
        Component* Clone(GameObject* newOwner) const override;

       private:
        States curStates = States::Idle;
        Animation* animPtr = nullptr;
        SpriteRenderer* sPtr = nullptr;
    };
}  // namespace mts

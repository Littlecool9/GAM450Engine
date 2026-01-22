/**
 * @file ActionList.h
 * @author Dong Ta
 * @brief  Action List Component
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <bitset>
#include <glm/vec3.hpp>

#include "Component.h"
#include "Actions/BaseAction.h"

namespace mts
{
    class ActionList : public Component
    {
       public:
        ActionList() {};
        ActionList(GameObject* owner);
        ~ActionList() override = default;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void LateUpdate(float dt) override;
        void Render() override;
        void Init() override;
        void SlowDown();
        void FastForward();
        void ResetSpeed();
        bool IsEmpty() const;
        void AddScaleAction(math::Vec3 Goal,
                            GameObject* target,
                            float TimeToComplete = 1,
                            int id = 0,
                            float delay = 0,
                            bool block = false,
                            BaseAction::EaseType ease = BaseAction::EaseType::Linear);
        void AddRotateAction(math::Vec3 Goal,
                             GameObject* target,
                             float TimeToComplete = 1,
                             int id = 0,
                             float delay = 0,
                             bool block = false,
                             BaseAction::EaseType ease = BaseAction::EaseType::Linear);
        void AddTranslateAction(math::Vec3 Goal,
                                GameObject* target,
                                float TimeToComplete = 1,
                                int id = 0,
                                float delay = 0,
                                bool block = false,
                                BaseAction::EaseType ease = BaseAction::EaseType::Linear);
        void AddAction(BaseAction* action);
        void Update(float dt) override;

       private:
        std::vector<BaseAction*> Actions;
        std::bitset<8> Groups;
        bool SpeedUp = false;
        bool SpeedDown = false;
        bool Paused = false;
        float Slow = 0.5f;             // how slow should the slowdown mode be
        float FastForwardSpeed = 5.f;  // how fast should the speed up mode should be
        float SpeedModifier = 1.f;
    };

}  // namespace mts

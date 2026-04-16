/**
 * @file EnemyStateParameter.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <variant>
#include "Engine/EntitySystem/Component/AnimationSystem.h"
namespace mts
{
    class EnemyAgent;
    class GameObject;

    class EnemyStateParameter
    {
       public:
        EnemyStateParameter() = default;
        virtual ~EnemyStateParameter() = default;
        virtual void Update(float dt, GameObject* owner) {};
        const std::variant<int, float, bool, AnimationSystem::States>& GetValue() const { return value; }
        template <typename T>
        void SetValue(T v)
        {
            value = v;
        }
       private:
        std::variant<int, float, bool, AnimationSystem::States> value;
        friend EnemyAgent;


    };

}  // namespace mts
/**
 * @file TestGameObjects.h
 * @author 11210
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    class TestGameObjects : public GameObject
    {
       public:
        TestGameObjects() = default;
        ~TestGameObjects() = default;
        virtual void Update(float dt) override;

       private:
    };

}  // namespace mts
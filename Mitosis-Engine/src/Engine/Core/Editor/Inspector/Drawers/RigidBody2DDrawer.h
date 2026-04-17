/**
 * @file RigidBody2DDrawer.h
 * @author sumin.park
 * @brief  drawer for rigidbody2d
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class RigidBody2DDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "RigidBody2D"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts
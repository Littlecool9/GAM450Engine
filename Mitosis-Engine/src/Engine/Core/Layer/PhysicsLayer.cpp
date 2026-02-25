/**
 * @file PhysicsLayer.cpp
 * @author sumin.park
 * @brief  Layer for physics system
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PhysicsLayer.h"

#include "Engine/Physics2D/PhysicsSystem2D.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    PhysicsLayer::PhysicsLayer() : Layer("PhysicsLayer")
    {
    }

    void PhysicsLayer::OnFixedUpdate(float fixedDt)
    {
        physics::PhysicsSystem2D::FixedUpdate(SceneSystem::GetCurrentScene(), fixedDt);
    }
}  // namespace mts
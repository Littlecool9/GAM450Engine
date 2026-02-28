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

#include "Engine/Core/Editor/EditorContext.h"
#include "Engine/Physics2D/PhysicsSystem2D.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    PhysicsLayer::PhysicsLayer(EditorContext& ctx) : Layer("PhysicsLayer"), mCtx(ctx)
    {
    }

    void PhysicsLayer::OnFixedUpdate(float fixedDt)
    {
        if (mCtx.playState == PlayState::Play)
            physics::PhysicsSystem2D::FixedUpdate(SceneSystem::GetCurrentScene(), fixedDt);
    }
}  // namespace mts
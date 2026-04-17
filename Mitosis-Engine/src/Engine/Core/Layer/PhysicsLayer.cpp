/**
 * @file PhysicsLayer.cpp
 * @author sumin.park
 * @brief  Layer for physics system
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
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

    void PhysicsLayer::OnAttach()
    {
        mSceneChangeId = SceneSystem::onChangeSceneDelegate.Subscribe(
            []() { physics::PhysicsSystem2D::Reset(); });
        Layer::OnAttach();
    }

    void PhysicsLayer::OnDetach()
    {
        SceneSystem::onChangeSceneDelegate.Unsubscribe(mSceneChangeId);
        Layer::OnDetach();
    }

    void PhysicsLayer::OnFixedUpdate(float fixedDt)
    {
        if (mCtx.playState == PlayState::Play)
            physics::PhysicsSystem2D::FixedUpdate(SceneSystem::GetCurrentScene(), fixedDt);
    }

    void PhysicsLayer::OnExitPlayMode()
    {
        physics::PhysicsSystem2D::Reset();
    }
}  // namespace mts
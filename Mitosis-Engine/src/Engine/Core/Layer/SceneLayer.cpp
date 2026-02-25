/**
 * @file SceneLayer.cpp
 * @author sumin.park
 * @brief  Layer for scene system
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SceneLayer.h"

#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    SceneLayer::SceneLayer(Scene* initialScene) : Layer("SceneLayer"), mInitialScene(initialScene)
    {
    }

    void SceneLayer::OnAttach()
    {
        SceneSystem::Init(mInitialScene);
        Layer::OnAttach();
    }

    void SceneLayer::OnDetach()
    {
        SceneSystem::Shutdown();
        Layer::OnDetach();
    }

    void SceneLayer::OnUpdate(float dt)
    {
        SceneSystem::Update(dt);
    }

    void SceneLayer::OnLateUpdate(float dt)
    {
        SceneSystem::LateUpdate(dt);
    }

    void SceneLayer::OnRender()
    {
        SceneSystem::Render();
    }

    void SceneLayer::OnDebugRender()
    {
        SceneSystem::DebugRender();
    }

    void SceneLayer::OnEndFrame()
    {
        SceneSystem::SceneCleanUp();
    }
}  // namespace mts
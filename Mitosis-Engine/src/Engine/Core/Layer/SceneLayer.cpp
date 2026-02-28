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

#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    SceneLayer::SceneLayer(Scene* initialScene, EditorContext& ctx)
        : Layer("SceneLayer"), mInitialScene(initialScene), mCtx(ctx)
    {
    }

    void SceneLayer::OnAttach()
    {
        GameObjectFactory::InitFactory();
        SceneSystem::Init(mInitialScene);
        Layer::OnAttach();
    }

    void SceneLayer::OnDetach()
    {
        SceneSystem::Shutdown();
        GameObjectFactory::ShutdownFactory();
        Layer::OnDetach();
    }

    void SceneLayer::OnUpdate(float dt)
    {
        if (mCtx.playState == PlayState::Play)
            SceneSystem::Update(dt);
    }

    void SceneLayer::OnLateUpdate(float dt)
    {
        if (mCtx.playState == PlayState::Play)
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
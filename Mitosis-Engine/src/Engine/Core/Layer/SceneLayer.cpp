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
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Serialization/SerializationUtility.h"

namespace mts
{
    SceneLayer::SceneLayer(Scene* initialScene, EditorContext& ctx)
        : Layer("SceneLayer"), mInitialScene(initialScene), mCtx(ctx)
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

    void SceneLayer::OnFixedUpdate(float fixedDt)
    {
        if (mCtx.playState == PlayState::Play)
            SceneSystem::FixedUpdate(fixedDt);
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

    void SceneLayer::OnEnterPlayMode()
    {
        Scene* scene = SceneSystem::GetCurrentScene();
        // Capture current (possibly unsaved) edit state
        std::string snapshot = SerializationUtility::SerializeToString(scene);
        // Preserve edit GOs untouched; deserialize fresh play GOs
        scene->StashGameObjects();
        SerializationUtility::DeserializeFromString(scene, snapshot);
        scene->EnterPlayMode();
    }

    void SceneLayer::OnExitPlayMode()
    {
        Scene* scene = SceneSystem::GetCurrentScene();
        scene->ExitPlayMode();        // stop audio / custom teardown
        scene->ClearGameObjects();    // delete play GOs
        scene->RestoreGameObjects();  // restore pristine edit GOs (no deserialize)
    }
}  // namespace mts
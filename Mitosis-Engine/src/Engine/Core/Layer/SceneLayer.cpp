/**
 * @file SceneLayer.cpp
 * @author sumin.park
 * @brief  Layer for scene system
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
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
    SceneLayer::SceneLayer(const std::string& scenePath,
                           const std::string& registryPath,
                           EditorContext& ctx)
        : Layer("SceneLayer"), mInitialScenePath(scenePath), mRegistryPath(registryPath), mCtx(ctx)
    {
    }

    void SceneLayer::OnAttach()
    {
        SceneSystem::Init(mInitialScenePath);
        SceneSystem::LoadRegistry(mRegistryPath);
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
        if (mCtx.playState == PlayState::Play)
            SceneSystem::ProcessPendingSceneSwitch();
    }

    void SceneLayer::OnEnterPlayMode()
    {
        Scene* scene = SceneSystem::GetCurrentScene();
        // Capture current (possibly unsaved) edit state
        std::string snapshot = SerializationUtility::SerializeToString(scene);
        // Preserve edit GOs untouched; deserialize fresh play GOs.
        // Deserialize does a plain promote — no lifecycle fires during it.
        scene->StashGameObjects();
        SerializationUtility::DeserializeFromString(scene, snapshot);
        // Set play mode before StartPlay so Start() guard passes.
        SceneSystem::SetInPlayMode(true);
        scene->StartPlay();
    }

    void SceneLayer::OnExitPlayMode()
    {
        Scene* scene = SceneSystem::GetCurrentScene();
        scene->ClearGameObjects();    // delete play GOs
        scene->RestoreGameObjects();  // restore pristine edit GOs (no deserialize)
        SceneSystem::SetInPlayMode(false);
    }
}  // namespace mts
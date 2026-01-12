/**
 * @file SceneSystem.cpp
 * @author sumin.park
 * @brief  The scene manager
 * @version 0.1
 * @date 1/8/2026 7:40:23 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "SceneSystem.h"
#include "Scene.h"

namespace rm
{
    Scene* SceneSystem::currentScene = nullptr;
    void SceneSystem::Init(Scene* initialScene) {
        ChangeCurrentScene(initialScene);
    }

    void SceneSystem::Update(float dt) {
        RM_ASSERT(currentScene != nullptr);
        currentScene->Update(dt);
    }

    void SceneSystem::LateUpdate(float dt) {
        RM_ASSERT(currentScene != nullptr);
        currentScene->LateUpdate(dt);
    }

    void SceneSystem::Render() {
        RM_ASSERT(currentScene != nullptr);
        currentScene->Render();
    }

    void SceneSystem::Shutdown() {
    }

    void SceneSystem::ChangeCurrentScene(Scene* newScene) {

        currentScene = newScene;
        currentScene->Init();
    }
} // rm namespace
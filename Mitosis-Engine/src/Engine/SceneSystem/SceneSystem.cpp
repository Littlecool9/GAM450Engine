/**
 * @file SceneSystem.cpp
 * @author sumin.park
 * @brief  The scene manager
 * @version 0.1
 * @date 1/8/2026 7:40:23 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SceneSystem.h"
#include "Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    std::vector<GameObject*> SceneSystem::dontDestroyObjects;
    Scene* SceneSystem::currentScene = nullptr;
    Scene* SceneSystem::nextScene = nullptr;
    Delegate<> SceneSystem::onChangeSceneDelegate;
    bool SceneSystem::isRestarting = false;
    bool SceneSystem::isChangingScene = false;

    void SceneSystem::Init(Scene* initialScene)
    {
        SetNextScene(initialScene);
        UpdateCurrentScene();
    }

    void SceneSystem::Update(float dt)
    {
        MTS_ASSERT(currentScene != nullptr);

        if (isChangingScene)
        {
            UpdateCurrentScene();
        }
        else if (isRestarting)
        {
            currentScene->Init();
            onChangeSceneDelegate.Invoke();
        }

        currentScene->Update(dt);
    }

    void SceneSystem::LateUpdate(float dt)
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->LateUpdate(dt);
    }

    void SceneSystem::Render()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->Render();
    }

    void SceneSystem::DebugRender()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->DebugRender();
    }

    void SceneSystem::Shutdown()
    {
    }

    // @brief: change the current scene if the next scene is set
    // @brief: this design require this function must be called before the SceneManager::Update
    void SceneSystem::UpdateCurrentScene()
    {
        if (nextScene != nullptr)
        {
            currentScene = std::move(nextScene);
            currentScene->Init();
            onChangeSceneDelegate.Invoke();

            isRestarting = false;
            isChangingScene = false;

            // add dont destroy objects to the new scene
            for (auto obj : dontDestroyObjects)
            {
                currentScene->CreateGameObject();  // Jackey TODO: Replace this part with a
                                                   // gameobject factory implementation
            }
        }
    }

    // @brief: set the next scene to change to
    // @brief: this is a public function allowing exterior systems to request scene changes
    void SceneSystem::SetNextScene(Scene* newScene)
    {
        nextScene = newScene;
        if (typeid(newScene) == typeid(currentScene))
        {
            isRestarting = true;
        }
        else
        {
            isChangingScene = true;
        }
    }

    void SceneSystem::AddDontDestroyObject(GameObject* obj)
    {
        obj->SetDontDestroy(true);
        dontDestroyObjects.push_back(obj);
    }

    void SceneSystem::RemoveDontDestroyObject(GameObject* obj)
    {
        obj->SetDontDestroy(false);
        // mind the performance of this operation
        dontDestroyObjects.erase(
            std::remove(dontDestroyObjects.begin(), dontDestroyObjects.end(), obj),
            dontDestroyObjects.end());
    }
}  // namespace mts
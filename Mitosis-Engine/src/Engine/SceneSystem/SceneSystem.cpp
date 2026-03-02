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
#include "Engine/Serialization/SerializationUtility.h"

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

    void SceneSystem::FixedUpdate(float dt)
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->FixedUpdate(dt);
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
        delete currentScene;
        for (auto obj : dontDestroyObjects)
        {
            delete obj;
        }
        dontDestroyObjects.clear();
    }

    void SceneSystem::SaveScene(Scene* scene, const std::string& fileName)
    {
        SerializationUtility::SerializeToFile(scene, fileName);
    }

    void SceneSystem::LoadScene(Scene* scene, const std::string& fileName)
    {
        SerializationUtility::DeserializeFromFile(scene, fileName);
        scene->SetFilePath(fileName);
    }

    void SceneSystem::SceneCleanUp()
    {
        MTS_ASSERT(currentScene != nullptr);
        currentScene->DestroyGameObjects();
    }

    // @brief: change the current scene if the next scene is set
    // @brief: this design require this function must be called before the SceneManager::Update
    void SceneSystem::UpdateCurrentScene()
    {
        if (nextScene != nullptr)
        {
            // Delete the old scene before switching to the new one
            delete currentScene;

            currentScene = nextScene;
            nextScene = nullptr;
            currentScene->Init();
            onChangeSceneDelegate.Invoke();

            isRestarting = false;
            isChangingScene = false;

            // add dont destroy objects to the new scene
            for (auto obj : dontDestroyObjects)
            {
                currentScene->AddGameObject(obj);
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

    // Transfer the ownership of the object to the scene system
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
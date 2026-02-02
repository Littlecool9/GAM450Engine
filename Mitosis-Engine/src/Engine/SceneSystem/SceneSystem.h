/**
 * @file SceneSystem.h
 * @author sumin.park
 * @brief  The scene manager
 * @version 0.1
 * @date 1/8/2026 7:40:23 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <functional>
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Scene;
    class GameObject;

    class SceneSystem
    {
       public:
        static void Init(Scene* initialScene);
        static void Update(float dt);
        static void LateUpdate(float dt);
        static void Render();
        static void DebugRender();
        static void Shutdown();
        static void SetNextScene(Scene* newScene);
        static Scene* GetCurrentScene() { return currentScene; }

        static void AddDontDestroyObject(GameObject* obj);
        static void RemoveDontDestroyObject(GameObject* obj);

       public:
        static Delegate<> onChangeSceneDelegate;

       private:
        static void UpdateCurrentScene();

       private:
        /*
         * @brief: The logic of dontDestroyObjects is :
         * 1. The gameplay logic register a gameobject to dontDestroyObjects before changing scene
         * 2. when deleting a scene, SceneSystem will skip deleting those objects
         * 3. when initializing a new scene, SceneSystem will re-add those objects to the new scene
         */
        static std::vector<GameObject*> dontDestroyObjects;

        static Scene* currentScene;
        static Scene* nextScene;

        static bool isRestarting;
        static bool isChangingScene;
    };

}  // namespace mts
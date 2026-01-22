/**
 * @file Scene.cpp
 * @author sumin.park
 * @brief  Scene that manages game objects
 * @version 0.1
 * @date 1/8/2026 7:42:45 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"

namespace mts
{
    Scene::~Scene() {
        for (auto g : gameObjects) {
            // Skip dontDestroy objects
            if (g->GetDontDestroy()) {
                continue;
            }
            delete g;
        }
    }

    void Scene::Init()
    {
        OnInit();
        for (auto g : gameObjects)
        {
            g->Init();
        }
    }

    void Scene::Update(float dt)
    {
        for (auto g : gameObjects)
        {
            g->Update(dt);
        }
    }

    void Scene::LateUpdate(float dt)
    {
        for (auto g : gameObjects)
        {
            g->LateUpdate(dt);
        }
    }

    void Scene::Render()
    {
        for (auto g : gameObjects)
        {
            g->Render();
        }
    }

    // Test purpose only, in gameplay will be replaced by the GameObjectFactory function
    GameObject* Scene::CreateGameObject() {

        //GameObject* newGameObject = GameObjectFactory::CreateGameObjectForTest(true);
        //gameObjects.push_back(newGameObject);

        GameObject* newGameObject2 = GameObjectFactory::CreateGameObjectForTest(false);
        gameObjects.emplace_back(newGameObject2);

        return newGameObject2;
    }
}  // namespace mts
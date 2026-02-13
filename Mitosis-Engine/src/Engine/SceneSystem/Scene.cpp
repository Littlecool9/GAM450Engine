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
    Scene::~Scene()
    {
        for (auto g : gameObjects)
        {
            // Skip dontDestroy objects
            if (g->GetDontDestroy())
            {
                continue;
            }
            delete g;
        }
    }

    void Scene::Init()
    {
        gameObjects.reserve(250);
        OnInit();
        for (auto g : gameObjects)
        {
            g->Init();
        }
    }

    void Scene::Update(float dt)
    {
        OnUpdate();
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

    void Scene::DebugRender()
    {
        for (auto g : gameObjects)
        {
            g->DebugRender();
        }
    }

    // Test purpose only, in gameplay will be replaced by the GameObjectFactory function
    GameObject* Scene::CreateGameObject()
    {
        GameObject* newGameObject = GameObjectFactory::CreateGameObject();
        return newGameObject;
    }

    GameObject* Scene::FindGameObjectUUID(const UUID& uuid)
    {
        for (auto g : gameObjects)
        {
            if (!g->IsDead() && g->GetId() == uuid)
                return g;
        }
        return nullptr;
    }

    void Scene::DestroyGameObjects()
    {
        for (auto it = gameObjects.begin(); it != gameObjects.end(); /* no increment here */)
        {
            if ((*it)->IsDead())
            {
                GameObject* g = *it;
                it = gameObjects.erase(it);
                delete g;
            }
            else
            {
                ++it;  // only increment if no element was erased
            }
        }
    }
}  // namespace mts
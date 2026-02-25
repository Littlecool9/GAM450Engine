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
        for (auto g : mGameObjects)
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
        mGameObjects.reserve(250);
        OnInit();
        for (auto g : mGameObjects)
        {
            g->Init();
        }
    }

    void Scene::Update(float dt)
    {
        OnUpdate();
        for (auto g : mGameObjects)
        {
            g->Update(dt);
        }
    }

    void Scene::LateUpdate(float dt)
    {
        for (auto g : mGameObjects)
        {
            g->LateUpdate(dt);
        }
    }

    void Scene::Render()
    {
        for (auto g : mGameObjects)
        {
            g->Render();
        }
    }

    void Scene::DebugRender()
    {
        for (auto g : mGameObjects)
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

    GameObject* Scene::FindGameObjectByUUID(const UUID& uuid)
    {
        for (auto g : mGameObjects)
        {
            if (!g->IsDead() && g->GetId() == uuid)
                return g;
        }
        return nullptr;
    }

    GameObject* Scene::FindGameObjectByName(const std::string& name)
    {
        for (auto g : mGameObjects)
        {
            if (!g->IsDead() && g->GetName() == name)
                return g;
        }
        return nullptr;
    }

    void Scene::DestroyGameObjects()
    {
        for (auto it = mGameObjects.begin(); it != mGameObjects.end(); /* no increment here */)
        {
            if ((*it)->IsDead())
            {
                GameObject* g = *it;
                it = mGameObjects.erase(it);
                delete g;
            }
            else
            {
                ++it;  // only increment if no element was erased
            }
        }
    }

    void Scene::Deserialize(JsonValue const& jsonObj)
    {
        JsonValue gameObjects = jsonObj["GameObjects"];
        assert(gameObjects.IsArray());
        size_t s = gameObjects.Size();
        mGameObjects.reserve(s * 2);
        for (size_t i = 0; i < s; ++i)
        {
            auto gv = gameObjects[i];
            assert(gv.IsObject());
            auto go = GameObjectFactory::CreateGameObject(this);
            go->Deserialize(gv);
        }
    }

    void Scene::Serialize(JsonDocument& doc) const
    {
        doc.BeginArray("GameObjects");
        for (auto g : mGameObjects)
        {
            doc.BeginObject();
            g->Serialize(doc);
            doc.EndObject();
        }
        doc.EndArray();
    }
}  // namespace mts
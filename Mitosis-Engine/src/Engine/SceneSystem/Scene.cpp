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
        for (auto g : mPendingGameObjects)
        {
            if (!g->GetDontDestroy())
                delete g;
        }
        for (auto g : mGameObjects)
        {
            // Skip dontDestroy objects
            if (g->GetDontDestroy())
            {
                continue;
            }
            delete g;
        }
        for (auto g : mEditGameObjects)
        {
            if (!g->GetDontDestroy())
                delete g;
        }
    }

    void Scene::Init()
    {
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
            if (g->IsActive())
                g->Update(dt);
        }
    }

    void Scene::LateUpdate(float dt)
    {
        for (auto g : mGameObjects)
        {
            if (g->IsActive())
                g->LateUpdate(dt);
        }
    }

    void Scene::FixedUpdate(float dt)
    {
        for (auto g : mGameObjects)
            if (g->IsActive())
                g->FixedUpdate(dt);
    }

    void Scene::Render()
    {
        for (auto g : mGameObjects)
        {
            if (g->IsActive())
                g->Render();
        }
    }

    void Scene::DebugRender()
    {
        for (auto g : mGameObjects)
        {
            if (g->IsActive())
                g->DebugRender();
        }
    }

    void Scene::EnterPlayMode()
    {
        for (auto* go : mGameObjects)
            if (go->IsActive())
                go->Init();
        OnEnterPlayMode();
    }

    void Scene::ExitPlayMode()
    {
        OnExitPlayMode();
    }

    // Test purpose only, in gameplay will be replaced by the GameObjectFactory function
    GameObject* Scene::CreateGameObject()
    {
        GameObject* newGameObject = GameObjectFactory::CreateEmptyGameObject();
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

    void Scene::FlushPendingGameObjects()
    {
        if (mPendingGameObjects.empty())
            return;
        mGameObjects.insert(mGameObjects.end(),
                            mPendingGameObjects.begin(),
                            mPendingGameObjects.end());
        mPendingGameObjects.clear();
    }

    void Scene::ClearGameObjects()
    {
        for (auto* go : mPendingGameObjects)
            delete go;
        mPendingGameObjects.clear();
        for (auto* go : mGameObjects)
            delete go;
        mGameObjects.clear();
    }

    void Scene::StashGameObjects()
    {
        // Discard any pending play GOs — they were created during play and must not
        // be stashed alongside edit GOs.
        for (auto* go : mPendingGameObjects)
            delete go;
        mPendingGameObjects.clear();
        mEditGameObjects = std::move(mGameObjects);
        mGameObjects.clear();
    }

    void Scene::RestoreGameObjects()
    {
        // Play GOs must already be cleared via ClearGameObjects()
        mGameObjects = std::move(mEditGameObjects);
        mEditGameObjects.clear();
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
        ClearGameObjects();
        if (!jsonObj.Has("GameObjects"))
            return;
        JsonValue gameObjects = jsonObj["GameObjects"];
        if (!gameObjects.IsArray())
            return;
        size_t s = gameObjects.Size();
        mGameObjects.reserve(s);
        for (size_t i = 0; i < s; ++i)
        {
            auto gv = gameObjects[i];
            assert(gv.IsObject());
            auto go = GameObjectFactory::CreateEmptyGameObject(this);
            go->Deserialize(gv);
        }
        FlushPendingGameObjects();
    }

    void Scene::Serialize(JsonDocument& doc) const
    {
        doc.BeginArray("GameObjects");
        for (auto g : mGameObjects)
        {
            // Skip serialization for TilemapCollider whose ownership isnt belong to the scene
            // Loading and Unloading tilemap colliders should be managed by Tilemap ResourceManager
            // which is not implemented yet,
            // This is a temp solution to skip serializing tilemap colliders until we implemented
            // the tilemap ResourceManager
            if (g->HasTag("TilemapCollider"))
            {
                continue;  // Skip serialization for tilemap collider objects
            }
            doc.BeginObject();
            g->Serialize(doc);
            doc.EndObject();
        }
        doc.EndArray();
    }
}  // namespace mts
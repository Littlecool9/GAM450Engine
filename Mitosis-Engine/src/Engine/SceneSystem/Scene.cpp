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
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    Scene::~Scene()
    {
        for (auto g : mGameObjects)
        {
            if (!g->GetDontDestroy())
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
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            mGameObjects[i]->Init();
    }

    void Scene::Awake()
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->Awake();
    }

    void Scene::OnEnable()
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->OnEnable();
    }

    void Scene::Start()
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->Start();
    }

    void Scene::StartPlay()
    {
        Init();
        Awake();
        OnEnable();
        Start();
    }

    void Scene::Update(float dt)
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->Update(dt);
    }

    void Scene::LateUpdate(float dt)
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->LateUpdate(dt);
    }

    void Scene::FixedUpdate(float dt)
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->FixedUpdate(dt);
    }

    void Scene::Render()
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->Render();
    }

    void Scene::DebugRender()
    {
        for (size_t i = 0, n = mGameObjects.size(); i < n; ++i)
            if (mGameObjects[i]->IsActive())
                mGameObjects[i]->DebugRender();
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

    void Scene::AddGameObject(GameObject* g)
    {
        mGameObjects.push_back(g);
        if (SceneSystem::IsInPlayMode())
        {
            g->Init();
            g->Awake();
            if (g->IsActive())
                g->OnEnable();
            if (g->IsActive())
                g->Start();
        }
    }

    void Scene::ClearGameObjects()
    {
        for (auto* go : mGameObjects)
            go->Destroy();
        for (auto* go : mGameObjects)
            delete go;
        mGameObjects.clear();
    }

    void Scene::StashGameObjects()
    {
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
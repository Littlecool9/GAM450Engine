/**
 * @file GameObjectFactory.cpp
 * @author Jackey Cen
 * @brief This is a Factory class to create GameObjects based on string or enum types.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "GameObjectFactory.h"
#include <unordered_map>
#include <functional>

#include "GameObject.h "
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Serialization/SerializationUtility.h"

#define ARCHETYPEPATH "Data/GameObjectFactoryArchetypes.json"
#define DEFAULTDESERIALIZEPATH "Data/"

namespace mts
{
    std::unordered_map<std::string, GameObject*> GameObjectFactory::archetypes;

    void GameObjectFactory::InitFactory()
    {
        JsonDocument doc = JsonDocument();
        if (!doc.LoadFromFile(ARCHETYPEPATH))
        {
            LOG_WARN("GameObjectFactory: Failed to load archetypes file '{}'", ARCHETYPEPATH);
            return;
        }

        JsonValue root = doc.Root();
        if (!root.Has("ArchetypesPaths"))
        {
            LOG_WARN(
                "GameObjectFactory: JSON root does not contain 'ArchetypesPaths', no archetypes "
                "will be loaded");
            return;
        }

        JsonValue archetypeArray = root["ArchetypesPaths"];
        if (archetypeArray.IsArray())
        {
            for (int i = 0; i < archetypeArray.Size(); ++i)
            {
                JsonValue archetype = archetypeArray[i];
                if (!archetype.Has("Name") || !archetype.Has("Path"))
                {
                    LOG_WARN("Archetype entry missing Name or Path, skipping");
                    continue;
                }
                std::string name = archetype["Name"].GetString("");
                std::string archetypePath = archetype["Path"].GetString("");
                if (!archetypePath.empty())
                {
                    archetypes[name] = BuildArchetypeFromJSON(archetypePath);
                    LOG_INFO("Loaded archetype {} from path {}", name, archetypePath);
                }
            }
        }
    }

    void GameObjectFactory::ShutdownFactory()
    {
        for (auto& [name, archetype] : archetypes)
        {
            delete archetype;
        }
        archetypes.clear();
    }

    // Return Empty GameObject
    GameObject* GameObjectFactory::CreateGameObject()
    {
        return CreateGameObject(SceneSystem::GetCurrentScene());
    }

    GameObject* GameObjectFactory::CreateGameObject(Scene* scene)
    {
        GameObject* obj = new GameObject();
        if (scene)
        {
            scene->AddGameObject(obj);
        }
        return obj;
    }

    GameObject* GameObjectFactory::InstantiateFromJSON(const std::string& path)
    {
        auto it = archetypes.find(path);
        if (it == archetypes.end())
            it = archetypes.emplace(path, BuildArchetypeFromJSON(path)).first;

        GameObject* newObj = new GameObject(*it->second);
        newObj->RegenerateId();
        if (Scene* scene = SceneSystem::GetCurrentScene())
            scene->AddGameObject(newObj);
        return newObj;
    }

    void GameObjectFactory::RemoveArchetype(const std::string& path)
    {
        auto it = archetypes.find(path);
        if (it != archetypes.end())
        {
            delete it->second;
            archetypes.erase(it);
        }
    }

    GameObject* GameObjectFactory::BuildArchetypeFromJSON(const std::string& filename)
    {
        auto go = new GameObject();
        mts::SerializationUtility::DeserializeFromFile(go, filename);
        return go;
    }

    GameObject* GameObjectFactory::FindArchetypeByName(const std::string& archetypeName)
    {
        auto res = archetypes.find(archetypeName);
        return res != archetypes.end() ? res->second : nullptr;
    }

    GameObject* GameObjectFactory::InstantiateFromArchetype(const std::string& archetypeName)
    {
        GameObject* go = FindArchetypeByName(archetypeName);
        if (go)
        {
            GameObject* newObj = new GameObject(*go);
            SceneSystem::GetCurrentScene()->AddGameObject(newObj);
            return newObj;
        }
        LOG_WARN("Archetype {} not found, trying to load from JSON", archetypeName);
        return nullptr;
    }

    GameObject* GameObjectFactory::Instantiate(const std::string& gameObjectName)
    {
        auto it = archetypes.find(gameObjectName);
        if (it == archetypes.end())
        {
            std::string jsonPath = DEFAULTDESERIALIZEPATH + gameObjectName + ".prefab";
            GameObject* archetype = BuildArchetypeFromJSON(jsonPath);
            if (archetype)
                it = archetypes.emplace(gameObjectName, archetype).first;
        }

        if (it != archetypes.end())
        {
            GameObject* newObj = new GameObject(*it->second);
            newObj->RegenerateId();
            if (Scene* scene = SceneSystem::GetCurrentScene())
                scene->AddGameObject(newObj);
            return newObj;
        }

        LOG_WARN("GameObject {} not found in archetypes or JSON, creating empty GameObject",
                 gameObjectName);
        return new GameObject();
    }

}  // namespace mts
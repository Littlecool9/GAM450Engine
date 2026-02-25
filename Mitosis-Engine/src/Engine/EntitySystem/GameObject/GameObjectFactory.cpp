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
        doc.LoadFromFile(ARCHETYPEPATH);
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
        scene->AddGameObject(obj);
        return obj;
    }

    GameObject* GameObjectFactory::InstantiateFromJSON(const std::string& filename)
    {
        //auto crate1 = new GameObject();
        GameObject* crate1 = CreateGameObject();
        mts::SerializationUtility::DeserializeFromFile(crate1, filename);
        return crate1;
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
        GameObject* newObj = InstantiateFromArchetype(gameObjectName);
        if (newObj == nullptr)
        {
            std::string jsonPath = DEFAULTDESERIALIZEPATH + gameObjectName + ".json";
            newObj = InstantiateFromJSON(jsonPath);
        }
        if (newObj == nullptr)
        {
            newObj = new GameObject();  
            LOG_WARN("GameObject {} not found in archetypes or JSON, creating empty GameObject",
                     gameObjectName);
        }

        return newObj;
    }

}  // namespace mts
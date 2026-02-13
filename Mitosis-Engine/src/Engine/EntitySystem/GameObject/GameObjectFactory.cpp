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

namespace mts
{
    // std::string const GameObjectFactory::deserializePath = "Assets/Serialization/GameObjects/";

    // Maps for string and enum to creator functions
    // Require manual registration of new GameObject types here
    std::unordered_map<std::string, std::function<GameObject*()>>
        GameObjectFactory::stringToTypeMap = {
            {"GameObject", []() -> GameObject* { return new GameObject(); }},
    };

    // Maps for string and enum to creator functions
    // Require manual registration of new GameObject types here
    // std::unordered_map<mts::GameObjectType, std::function<GameObject*()>>
    //    GameObjectFactory::typeToCreatorMap = {
    //        {mts::GameObjectType::GameObject, []() -> GameObject* { return new GameObject(); }},
    //        {mts::GameObjectType::TestGameObjects, []() -> GameObject* { return new
    //        TestGameObjects(); }},
    //};

    std::vector<GameObject*> GameObjectFactory::archetypes;

    GameObject* GameObjectFactory::CreateGameObjectFromArchetype(const std::string& gameObjectName)
    {
        GameObject* archetype = nullptr;
        for (auto GameObject : archetypes)
        {
            if (GameObject->GetName() == gameObjectName)
            {
                archetype = GameObject;
            }
        }

        if (archetype != nullptr)
        {
            // Return a copy of the archetype
            return new GameObject(*archetype);
        }
        else
        {
            // Failed to find in archetypes, create new one
            GameObject* obj = new GameObject();  // stringToTypeMap[gameObjectName]();

            // Deserialize from file
            JsonDocument doc = JsonDocument();
            std::string filepath = gameObjectName;
            bool res = doc.LoadFromFile(filepath);
            if (!res)
            {
                LOG_ERROR("Failed to load GameObject deserialization file: {}", filepath);
                return nullptr;
            }
            obj->Deserialize(doc.Root());

            return obj;
        }
        return nullptr;
    }

    // Return Empty GameObject
    GameObject* GameObjectFactory::CreateGameObject()
    {
        GameObject* obj = new GameObject();
        Scene* currentScene = SceneSystem::GetCurrentScene();
        if (currentScene)
        {
            LOG_INFO("Created GameObject {} and added to current scene", obj->GetName());
            currentScene->AddGameObject(obj);
        }
        return obj;
    }

    GameObject* GameObjectFactory::CreateGameObject(const std::string& file)
    {
        auto crate1 = CreateGameObject();
        mts::SerializationUtility::DeserializeFromFile(crate1, file);
        return crate1;
    }

}  // namespace mts
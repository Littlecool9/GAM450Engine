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
#include "TestGameObjects.h"

namespace mts
{
    std::string const GameObjectFactory::deserializePath = "Assets/Serialization/GameObjects/";

    // Maps for string and enum to creator functions
    // Require manual registration of new GameObject types here
    std::unordered_map<std::string, std::function<GameObject*()>>
        GameObjectFactory::stringToTypeMap = {
            {"GameObject", []() -> GameObject* { return new GameObject(); }},
            {"TestGameObjects", []() -> GameObject* { return new TestGameObjects(); }},
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

    GameObject* GameObjectFactory::CreateGameObjectImpl(const std::string& gameObjectName)
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
            GameObject* obj = stringToTypeMap[gameObjectName]();

            // Deserialize from file
            JsonDocument doc = JsonDocument();
            std::string filepath = deserializePath + gameObjectName + ".json";
            bool res = doc.LoadFromFile(filepath);
            if (!res)
            {
                LOG_ERROR("Failed to load GameObject deserialization file: {}", filepath);
                return nullptr;
            }
            obj->Deserialize(doc.Root());

            archetypes.emplace_back(obj);
            return new GameObject(*obj);
        }
        return nullptr;
    }

    GameObject* GameObjectFactory::CreateGameObject(const std::string& type)
    {
        return CreateGameObjectImpl(type);
    }

    // GameObject* GameObjectFactory::CreateGameObject(const mts::GameObjectType type)
    //{
    //     return CreateGameObjectImpl(typeToCreatorMap, type,
    //     std::to_string(static_cast<int>(type)));
    // }

    // Create GameObject for testing purposes
    // remove it when it is no longer needed
    GameObject* GameObjectFactory::CreateGameObjectForTest(bool test)
    {
        if (test)
        {
            return new TestGameObjects();
        }
        return new GameObject();
    }


}  // namespace mts
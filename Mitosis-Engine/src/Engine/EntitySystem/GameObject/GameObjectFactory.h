/**
 * @file GameObjectFactory.h
 * @author Jackey Cen
 * @brief This header defines the GameObjectFactory class, which is responsible for creating
 * GameObjects.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <functional>
#include <string>
#include <unordered_map>

namespace mts
{
    class Scene;
    // Forward declaration
    class GameObject;

    enum class GameObjectType : uint16_t
    {
        GameObject,
        TestGameObjects,
        TilemapColliderObject,
    };

    class GameObjectFactory
    {
       public:
        static void ShutdownFactory();

        // Instantiate from archetype/prefabs, if not a archetype/prefab, it will create a new one
        // from json file
        static GameObject* Instantiate(const std::string& gameObjectName);
        // Return Empty GameObject
        static GameObject* CreateGameObject();
        static GameObject* CreateGameObject(Scene* scene);

        // Temporary set to be public for testing, will be removed after the scene is cleared
        // Build GameObject from json file and return
        static GameObject* InstantiateFromJSON(const std::string& filename);

        // Remove a cached archetype by path so it is reloaded from disk on next instantiation
        static void RemoveArchetype(const std::string& path);

       private:
        // Load GameObject from archetype
        static GameObject* InstantiateFromArchetype(const std::string& archetypeName);
        // Build Archetype from json file and add it to the archetype list, return the archetype
        static GameObject* BuildArchetypeFromJSON(const std::string& filename);
        static GameObject* FindArchetypeByName(const std::string& archetypeName);

       private:
        // Holds archetypes that will never be used in runtime
        // The key is the name of the archetype, and the value is the GameObject pointer of the
        // archetype
        static std::unordered_map<std::string, GameObject*> archetypes;
        static std::string const deserializePath;
    };

}  // namespace mts
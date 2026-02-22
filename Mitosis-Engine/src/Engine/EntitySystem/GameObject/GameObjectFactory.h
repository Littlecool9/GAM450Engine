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
        // @brief: Create GameObject by string type
        // @param type: name of the GameObject
        // @return: pointer to created GameObject, nullptr if failed
        static GameObject* CreateGameObject(const std::string& type);
        static GameObject* CreateGameObject();
        static GameObject* CreateGameObject(Scene* scene);

        // @brief: Create GameObject by enum type
        // @param type: enum type of the GameObject
        // @return: pointer to created GameObject, nullptr if failed
        // static GameObject* CreateGameObject(const mts::GameObjectType type);

       private:
        // @brief: Actual creator functions
        // @brief: Template function to create GameObject from given map and key
        // @param key: key to find in the map (string or enum(not implemented))
        // @param typeString: string representation of the type for deserialization file lookup
        static GameObject* CreateGameObjectFromArchetype(const std::string& typeString);

        static std::unordered_map<std::string, std::function<GameObject*()>> stringToTypeMap;
        // static std::unordered_map<mts::GameObjectType, std::function<GameObject*()>>
        // typeToCreatorMap;

        static std::vector<GameObject*> archetypes;
        static std::string const deserializePath;
    };

}  // namespace mts
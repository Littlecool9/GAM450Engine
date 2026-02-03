/**
 * @file Scene.h
 * @author sumin.park
 * @brief  Scene that manages game objects
 * @version 0.1
 * @date 1/8/2026 7:42:45 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    class GameObject;
    class UUID;
    class Scene
    {
        // Jackey TODO: multiple layers support
       public:
        Scene() = default;
        virtual ~Scene();

        void Init();
        void Update(float dt);
        void LateUpdate(float dt);
        void Render();
        void DebugRender();

        std::vector<GameObject*>& GetGameObjects() { return gameObjects; }

        GameObject* CreateGameObject();
        GameObject* FindGameObjectUUID(const UUID& uuid);

        // TODO: REMOVE, TEMPORARY SETTER FOR TESTING
        void AddGameObject(GameObject* g) { gameObjects.push_back(g); }

        void DestroyGameObjects();

       protected:
        virtual void OnInit() = 0;
        virtual void OnUpdate() {};  // testing purposes

       protected:
        std::vector<GameObject*> gameObjects;
    };

}  // namespace mts
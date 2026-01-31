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

        std::vector<GameObject*>& GetGameObjects() { return gameObjects; }

        GameObject* CreateGameObject();

        // TODO: REMOVE, TEMPORARY SETTER FOR TESTING
        void AddGameObject(GameObject* g) { gameObjects.push_back(g); }

       protected:
        virtual void OnInit() = 0;

       protected:
        std::vector<GameObject*> gameObjects;
    };

}  // namespace mts
/**
 * @file Scene.h
 * @author sumin.park
 * @brief  Scene that manages game objects
 * @version 0.1
 * @date 1/8/2026 7:42:45 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

namespace rm {
    class GameObject;
    class Scene {
       public:
        Scene() = default;
        virtual ~Scene();

        void Init();
        void Update(float dt);
        void LateUpdate(float dt);
        void Render();
        
        GameObject* CreateGameObject();
    protected:
        virtual void OnInit() = 0;
       private:
        std::vector<GameObject*> gameObjects;
    };

}  // namespace rm
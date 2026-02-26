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
#include "Engine/Serialization/Deserializable.h"

namespace mts
{
    class GameObject;
    class UUID;
    class Scene : public Deserializable
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

        std::vector<GameObject*>& GetGameObjects() { return mGameObjects; }

        GameObject* CreateGameObject();
        GameObject* FindGameObjectByUUID(const UUID& uuid);
        GameObject* FindGameObjectByName(const std::string& name);

        // TODO: REMOVE, TEMPORARY SETTER FOR TESTING
        void AddGameObject(GameObject* g) { mGameObjects.push_back(g); }

        void DestroyGameObjects();

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

       protected:
        virtual void OnInit() = 0;
        virtual void OnUpdate() {}

       protected:
        std::vector<GameObject*> mGameObjects;
    };

}  // namespace mts
/**
 * @file Scene.h
 * @author sumin.park
 * @brief  Scene that manages game objects
 * @version 0.1
 * @date 1/8/2026 7:42:45 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Serialization/Deserializable.h"

namespace mts
{
    class GameObject;
    class UUID;
    class Scene final : public Deserializable
    {
        // Jackey TODO: multiple layers support
       public:
        Scene() = default;
        ~Scene();

        void Init();
        void Awake();     // play mode only, broadcast before Start
        void OnEnable();  // play mode only, fires for all active GOs after Awake
        void Start();     // play mode only, calls Start on all active GOs
        void StartPlay(); // convenience: Init → Awake → OnEnable → Start
        void Update(float dt);
        void LateUpdate(float dt);
        void FixedUpdate(float dt);
        void Render();
        void DebugRender();

        std::vector<GameObject*>& GetGameObjects() { return mGameObjects; }

        const std::string& GetFilePath() const { return mFilePath; }
        void SetFilePath(const std::string& path) { mFilePath = path; }

        GameObject* CreateGameObject();
        GameObject* FindGameObjectByUUID(const UUID& uuid);
        GameObject* FindGameObjectByName(const std::string& name);

        void AddGameObject(GameObject* g);

        void DestroyGameObjects();
        void ClearGameObjects();

        void StashGameObjects();  // move mGameObjects → mEditGameObjects (call before deserialize)
        void RestoreGameObjects();  // clear mGameObjects, move mEditGameObjects back

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

       private:
        std::vector<GameObject*> mGameObjects;
        std::string mFilePath;
        std::vector<GameObject*> mEditGameObjects;
    };

}  // namespace mts
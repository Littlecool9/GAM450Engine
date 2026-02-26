/**
 * @file ScriptBase.h
 * @author rahul
 * @brief Base class for user-authored gameplay scripts with Unity-style lifecycle callbacks.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    class JsonDocument;
    class GameObject;
    class JsonValue;

    class ScriptBase
    {
       public:
        virtual ~ScriptBase() = default;

        void SetOwner(GameObject* go) { owner = go; }
        GameObject* GetGameObject() const { return owner; }

        // Lifecycle
        virtual void OnCreate() {}
        virtual void OnUpdate(float /*dt*/) {}
        virtual void OnLateUpdate(float /*dt*/) {}
        virtual void OnEnable() {}
        virtual void OnDisable() {}

        // Data-driven script fields
        virtual void DeserializeFields(JsonValue const& /*fields*/) {}
        virtual void SerializeFields(mts::JsonDocument& out) const {}

        // Collision hooks (matches the GameObject collision callbacks)
        virtual void OnCollisionEnter2D(const GameObject* /*other*/) {}
        virtual void OnCollisionStay2D(const GameObject* /*other*/) {}
        virtual void OnCollisionExit2D(const GameObject* /*other*/) {}

       private:
        GameObject* owner = nullptr;
    };

}  // namespace mts
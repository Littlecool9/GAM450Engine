/**
 * @file ScriptBase.h
 * @author rahul
 * @brief Base class for user-authored gameplay scripts with Unity-style lifecycle callbacks.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <functional>
#include <vector>

namespace mts
{
    class JsonDocument;
    class GameObject;
    class JsonValue;
    class UUID;
    class Component;
    struct Collision2D;

    struct ScriptField
    {
        enum class Type
        {
            Float,
            Int,
            Bool,
            GameObject,
            Component
        };
        const char* name = nullptr;
        Type type = Type::Float;
        union
        {
            float* f;
            int* i;
            bool* b;
            GameObject** go;
            Component** comp;
        } ptr{};
        UUID* uuid = nullptr;  // points to script's UUID member; required to persist
        std::function<Component*(GameObject*)> resolver;  // for Type::Component only
        float step = 0.05f;
    };

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
        virtual void OnFixedUpdate(float /*dt*/) {}
        virtual void OnEnable() {}
        virtual void OnDisable() {}

        // Data-driven script fields
        virtual void DeserializeFields(JsonValue const& /*fields*/) {}
        virtual void SerializeFields(mts::JsonDocument& out) const {}

        // Inspector field exposure
        virtual void GetInspectorFields(std::vector<ScriptField>& /*fields*/) {}

        // Collision hooks (matches the GameObject collision callbacks)
        virtual void OnCollisionEnter2D(const Collision2D& collision) {};
        virtual void OnCollisionStay2D(const Collision2D& collision) {};
        virtual void OnCollisionExit2D(const Collision2D& collision) {};

        virtual const std::string& GetClassName() const = 0;

       private:
        GameObject* owner = nullptr;
    };

}  // namespace mts

// Injects GetClassName() override and StaticClassName() accessor into a script class.
// Usage: place SCRIPT_CLASS(MyScriptName) in the public section of the class body.
#define SCRIPT_CLASS(ClassName)                      \
   public:                                           \
    static const std::string& StaticClassName()      \
    {                                                \
        static const std::string kName = #ClassName; \
        return kName;                                \
    }                                                \
    const std::string& GetClassName() const override \
    {                                                \
        return StaticClassName();                    \
    }
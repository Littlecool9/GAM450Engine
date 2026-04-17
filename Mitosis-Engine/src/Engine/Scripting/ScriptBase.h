/**
 * @file ScriptBase.h
 * @author rahul
 * @brief Base class for user-authored gameplay scripts with Unity-style lifecycle callbacks.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include <functional>
#include <vector>
#include <string>

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
            Vec2,
            Vec3,
            Color,
            String,
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
            glm::vec2* v2;
            glm::vec3* v3;
            std::string* s;
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
        virtual void Init() {
        }  // once at scene load (edit or play). Cache own-GO component pointers here.
        virtual void Awake() {}  // play mode only, fires before Start. Use for conductor
                                 // subscriptions and singleton setup.
        virtual void Start() {}  // play mode only, fires the first time the GO is activated. Use
                                 // for cross-GO UUID resolve.
        virtual void OnUpdate(float /*dt*/) {}
        virtual void OnLateUpdate(float /*dt*/) {}
        virtual void OnFixedUpdate(float /*dt*/) {}
        virtual void OnRender() {}
        virtual void OnEnable() {}
        virtual void OnDisable() {}
        virtual void OnDestroy() {}  // play mode only — on destruction while in play.

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
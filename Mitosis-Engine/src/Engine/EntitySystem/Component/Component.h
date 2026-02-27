/**
 * @file Component.h
 * @author sumin.park
 * @brief  Basic Component class
 * @version 0.1
 * @date 1/8/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Serialization/Deserializable.h"
#include "Engine/Delegate/Delegate.h"
#include "Engine/Memory/MemoryManager.h"

namespace mts
{
    class ComponentFactory;
    class GameObject;

    class Component : public Deserializable
    {
       public:
        virtual ~Component() = default;

        Component& operator=(const Component& other) = default;

        MTS_CLASS_NEW_DELETE();

       public:
        virtual void Init() {}
        virtual void Update(float dt) {}
        virtual void LateUpdate(float dt) {}
        virtual void Render() {}
        virtual void DebugRender() {}

        virtual void OnCollisionEnter2D(const GameObject* /*other*/) {}
        virtual void OnCollisionStay2D(const GameObject* /*other*/) {}
        virtual void OnCollisionExit2D(const GameObject* /*other*/) {}

        void Deserialize(JsonValue const& jsonObj) override;

        GameObject* GetGameObject() const { return owner; }
        GameObject* GetGameObject() { return owner; }

        Delegate<> onEnable;
        Delegate<> onDisable;

        bool IsEnabled() const { return isEnabled; }
        void SetEnabled(bool enabled);

       protected:
        Component();
        Component(GameObject* owner);
        Component(const Component& other) = default;

       private:
        friend class ComponentFactory;
        friend class GameObject;
        GameObject* owner;

        bool isDeserialized;

       protected:
        // Component properties

        // @brief: Whether the component is enabled or not
        // @brief: This is an extremely complicated system design when using it in gameplay logic
        // @brief: This is now used temporarily for engine level enabling/disabling only
        bool isEnabled;
    };

}  // namespace mts

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

namespace mts
{
    class ComponentFactory;
    class GameObject;

    class Component : public Deserializable
    {
       public:
        virtual ~Component() = default;

        Component& operator=(const Component& other) = default;

        //static void* operator new(std::size_t size,
        //                          std::source_location loc = std::source_location::current())
        //{
        //    return MTS_ALLOC_T(Component);
        //}
        //static void operator delete(void* ptr) { MTS_FREE(ptr); }

        //static void operator delete(void* ptr, std::source_location) { MTS_FREE(ptr); }

       public:
        virtual void Init() = 0;
        virtual void Update(float dt) = 0;
        virtual void LateUpdate(float dt) = 0;
        virtual void Render() = 0;

        virtual void Deserialize(JsonValue const& jsonObj) override;
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
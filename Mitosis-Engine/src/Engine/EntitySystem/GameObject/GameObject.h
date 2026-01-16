/**
 * @file GameObject.h
 * @author sumin.park
 * @brief  GameObject
 * @version 0.1
 * @date 1/8/2026 7:30:10 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Serialization/Deserializable.h"

namespace mts
{
    class Component;
    class ComponentFactory;
    class GameObject: public Deserializable
    {
       public:
        GameObject();
        GameObject(ComponentFactory* obj);
        virtual ~GameObject();

       public:
        void Init();
        void Update(float dt);
        void LateUpdate(float dt);
        void Render();

        // Deserializable interface
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        //end

        template <typename T>
        T* AddComponent()
        {
            // todo: check if component already exists
            // todo: also check if T is actually a type of component
            T* newComponent = new T(this);
            components.push_back(newComponent);
            return newComponent;
        }

        template <typename T>
        T* GetComponent()
        {
            for (auto c : components)
            {
                T* target = dynamic_cast<T*>(c);
                if (target != nullptr)
                    return target;
            }
            return nullptr;
        }

        void SetDontDestroy(bool value) { dontDestroy = value; }
        bool GetDontDestroy() const { return dontDestroy; }
	private:
        std::vector<Component*> components;
        // pointer to component factory for deserialization, to be deleted/removed once we figure
        // out where we want this component factory to stay, e.g. scene/system level singleton, etc.
        // then we can remove this and change the deserialize code accordingly
        ComponentFactory* componentFactory; 
        bool dontDestroy = false;
	};
}  // namespace mts
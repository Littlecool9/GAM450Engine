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
#include "Engine/EntitySystem/Component/ComponentFactory.h"
#include "Engine/EntitySystem/Component/Component.h"
#include "Engine/Entity/Entity.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mts
{
    // forward declarations
    class JsonValue;
    class Deserializable;

    // Tag is a lightweight identifier
    // This class is not hashable in this implementation
    struct Tag
    {
        uint32_t id;
        bool operator==(const Tag&) const = default;
    };
    class TagRegistry
    {
       public:
        static Tag GetOrCreate(const std::string& name);

       private:
        static std::unordered_map<std::string, Tag> nameToTag;
        static uint32_t nextId;
    };

    class GameObject : public Deserializable, public Entity
    {
       public:
        virtual ~GameObject();

        // May need to include header file here
        MTS_CLASS_NEW_DELETE();

       public:
        virtual void Init();
        virtual void Update(float dt);
        virtual void LateUpdate(float dt);
        virtual void Render();
        void Destroy();

        // One Object can only be in one layer at a time
        enum LayerMasks
        {
            Default,
            UI,
        };

        // Deserializable interface
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        // end

        template <typename T>
        T* AddComponent()
        {
            if (GetComponent<T>() != nullptr)
            {
                LOG_WARN("GameObject {} already has component of type {}", name, typeid(T).name());
                return nullptr;
            }
            Component* newComp = ComponentFactory::CreateComponent<T>();
            if (newComp == nullptr)
            {
                LOG_ERROR("Failed to create component of type {}", typeid(T).name());
                return nullptr;
            }
            components.emplace_back(newComp);
            LOG_INFO("Added component of type {} to GameObject {}", typeid(T).name(), name);    
            newComp->owner = this;
            T* res = dynamic_cast<T*>(newComp);

            return res;
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
        // Getters and Setters
        inline void SetDontDestroy(bool value) { dontDestroy = value; }
        inline bool GetDontDestroy() const { return dontDestroy; }
        inline bool IsDead() const { return isDead; }
        inline bool IsActive() const { return isActive; }
        inline bool SetActive(bool active) { return isActive = active; }
        inline void SetLayer(LayerMasks layer) { objectLayer = layer; }
        inline void SetName(std::string const& name) { this->name = name; }
        inline std::string const& GetName() const { return name; }
        inline std::vector<GameObject*> const& GetChildren() const { return children; }
        GameObject* GetChildByName(const std::string& childName) const;
        inline void AddChild(GameObject* child) { children.emplace_back(child); }
        bool HasTag(const std::string& tagName) const;
        void AddTag(const std::string& tagName);

       protected:
        // @brief: GameObject constructor is protected to enforce usage of GameObjectFactory
        GameObject();
        GameObject(const GameObject& other);
        GameObject(const std::string& name);

        std::vector<Component*> components;
        std::vector<GameObject*> children;

       private:
        friend class GameObjectFactory;

        bool isDeserialized = false;

        // GameObject properties
        // (preferably initialized in the deserialization process)
        // (or modified in gameplay)

        bool dontDestroy;
        bool isActive;
        bool isDead = false;  // in the queue for destruction;

        // @brief: Name can be non-unique
        std::string name;
        // @brief: One GameObject can only be in one layer at a time
        LayerMasks objectLayer = LayerMasks::Default;
        // @brief: One GameObject can have multiple tags
        std::vector<Tag> tags;
    };

}  // namespace mts

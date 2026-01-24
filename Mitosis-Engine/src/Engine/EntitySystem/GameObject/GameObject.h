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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "Engine/Serialization/Deserializable.h"
#include "Engine/EntitySystem/Component/ComponentFactory.h"
#include "Engine/EntitySystem/Component/Component.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mts
{
    // forward declarations
    class JsonValue;
    class Deserializable;

    // @brief: Multi-layer rendering requires pre-process in the camera and renderer.
    class GameObjectId
    {
       public:
        GameObjectId() : id_(boost::uuids::random_generator()()) {}
        explicit GameObjectId(const boost::uuids::uuid& uuid) : id_(uuid) {}

        std::string toString() const { return boost::uuids::to_string(id_); }

        bool operator==(const GameObjectId& other) const { return id_ == other.id_; }

        bool operator!=(const GameObjectId& other) const { return id_ != other.id_; }

        bool operator<(const GameObjectId& other) const { return id_ < other.id_; }

        const boost::uuids::uuid& uuid() const { return id_; }

       private:
        boost::uuids::uuid id_;
    };

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

    class GameObject : public Deserializable
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

        // One Object can only be in one layer at a time
        enum LayerMasks
        {
            Default,
            UI,
        };

        // Deserializable interface
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        //end

        template <typename T>
        T* AddComponent()
        {
            // todo: check if component already exists
            // todo: also check if T is actually a type of component
            Component* newComp = ComponentFactory::CreateComponent<T>();
            if (newComp == nullptr)
            {
                LOG_ERROR("Failed to create component of type {}", typeid(T).name());
                return nullptr;
            }
            components.emplace_back(newComp);
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
        inline const std::string GetIdString() const { return id_.toString(); }
        inline void SetDontDestroy(bool value) { dontDestroy = value; }
        inline bool GetDontDestroy() const { return dontDestroy; }
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

        GameObjectId id_;
        bool isDeserialized = false;

        // GameObject properties 
        // (preferably initialized in the deserialization process)
        // (or modified in gameplay)

        bool dontDestroy;
        bool isActive;

        // @brief: Name can be non-unique
        std::string name;
        // @brief: One GameObject can only be in one layer at a time
        LayerMasks objectLayer = LayerMasks::Default;
        // @brief: One GameObject can have multiple tags
        std::vector<Tag> tags;
    };

}  // namespace mts

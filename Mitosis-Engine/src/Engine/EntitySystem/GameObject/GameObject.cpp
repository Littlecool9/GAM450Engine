/**
 * @file GameObject.cpp
 * @author sumin.park
 * @brief  GameObject
 * @version 0.1
 * @date 1/8/2026 7:30:10 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "GameObject.h"
#include "../Component/Component.h"
#include "../Component/Transform.h"
#include "../Component/ComponentFactory.h"
namespace mts
{
    std::unordered_map<std::string, Tag> TagRegistry::nameToTag;
    uint32_t TagRegistry::nextId = 0;

    GameObject::GameObject() : dontDestroy(false)
    {
    }

    GameObject::GameObject(const GameObject& other)
        : name(other.name),
          dontDestroy(other.dontDestroy),
          isActive(other.isActive),
          objectLayer(other.objectLayer),
          tags(other.tags)
    {
    }

    GameObject::GameObject(const std::string& name) : name(name), dontDestroy(false), isActive(true)
    {
    }

    GameObject::~GameObject()
    {
        for (auto c : components)
            delete c;
    }

    void GameObject::Deserialize(JsonValue const& jsonObj)
    {
        /*An example if gameobject were to have a name
        if (jsonObj.Has("Name"))
        {
            SetName(jsonObj["Name"].GetString(""));
        }
         */

        if (!jsonObj.Has("Components"))
        {
            return;
        }
        JsonValue comps = jsonObj["Components"];
        if (!comps.IsArray())
        {
            return;
        }
        size_t compCount = comps.Size();
        for (size_t i = 0; i < compCount; ++i)
        {
            JsonValue item = comps[i];
            if (!item.IsObject())
                continue;

            // Must have "Type"
            if (!item.Has("Type"))
                continue;

            const char* typeName = item["Type"].GetString("");
            if (!typeName || typeName[0] == '\0')
                continue;

            auto comp = ComponentFactory::CreateComponent(typeName);
            if (!comp)
                continue;

            // "Data" is optional depending on component
            if (item.Has("Data"))
            {
                comp->Deserialize(item["Data"]);
            }
            comp->owner = this;
            components.emplace_back(comp);
        }
    }

    void GameObject::Serialize(JsonDocument& json) const
    {
        // If you have name/id/etc
        // doc.WriteString("Name", name.c_str()); //or use writeint/writefloat if it's an id, or use
        // string and do logic to read later
        json.BeginArray("Components");  // "Components": [ ... ]
        for (auto c : components)
        {
            json.BeginObject();  // {
            c->Serialize(json);
            json.EndObject();  // }
        }
        json.EndArray();  // end "Components"
    }

#define FOR_CONDITION_COMPONENTS(cond, action) \
    for (auto c : components)                  \
        if (c->cond)                           \
    action

    void GameObject::Init()
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->Init());
    }

    void GameObject::Update(float dt)
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->Update(dt));
    }

    void GameObject::LateUpdate(float dt)
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->LateUpdate(dt));
    }

    void GameObject::Render()
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->Render());
    }

    void GameObject::DebugRender()
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->DebugRender());
    }

    void GameObject::Destroy()
    {
        isDead = true;
    }

    GameObject* GameObject::GetChildByName(const std::string& childName) const
    {
        for (auto child : children)
        {
            if (child->GetName() == childName)
                return child;
        }
        return nullptr;
    }

    void GameObject::AddTag(const std::string& tagName)
    {
        Tag tag = TagRegistry::GetOrCreate(tagName);
        tags.emplace_back(tag);
    }

    bool GameObject::HasTag(const std::string& tagName) const
    {
        Tag tag = TagRegistry::GetOrCreate(tagName);
        return std::find(tags.begin(), tags.end(), tag) != tags.end();
    }

    Tag TagRegistry::GetOrCreate(const std::string& name)
    {
        auto it = nameToTag.find(name);
        if (it != nameToTag.end())
            return it->second;

        Tag tag{nextId++};
        nameToTag.emplace(name, tag);
        return tag;
    }

}  // namespace mts
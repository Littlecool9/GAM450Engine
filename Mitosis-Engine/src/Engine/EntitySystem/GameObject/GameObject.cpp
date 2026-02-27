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
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"

namespace mts
{
    std::unordered_map<std::string, Tag> TagRegistry::nameToTag;
    std::vector<std::string> TagRegistry::tagToName;
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
        components.reserve(other.components.size());
        for (Component* c : other.components)
            components.push_back(c->Clone(this));
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
        if (jsonObj.Has("UUID"))
        {
            _id = UUID::FromString(jsonObj["UUID"].GetString());
        }
        if (jsonObj.Has("Name"))
        {
            SetName(jsonObj["Name"].GetString(""));
        }
        if (jsonObj.Has("Layer"))
        {
            SetLayer(static_cast<LayerMasks>(jsonObj["Layer"].GetInt()));
        }

        if (jsonObj.Has("Tags"))
        {
            JsonValue arr = jsonObj["Tags"];
            if (arr.IsArray())
            {
                tags.clear();
                for (size_t i = 0; i < arr.Size(); ++i)
                {
                    const char* name = arr[i].GetString("");
                    if (!name || name[0] == '\0')
                        continue;
                    tags.insert(TagRegistry::GetOrCreate(name));
                }
            }
        }

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

            comp->owner = this;

            if (item.Has("Data"))
            {
                comp->Deserialize(item["Data"]);
            }

            components.emplace_back(comp);
        }
    }

    void GameObject::Serialize(JsonDocument& json) const
    {
        json.WriteString("UUID", _id.ToString().c_str());
        json.WriteString("Name", name.c_str());
        json.WriteInt("Layer", static_cast<int>(objectLayer));

        json.BeginArray("Tags");
        for (const Tag& t : tags)
            json.WriteString(TagRegistry::GetName(t).c_str());
        json.EndArray();

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

    void GameObject::OnCollisionEnter2D(const GameObject* other)
    {
        // go down scripts and call OnCollisionEnter for all of them.
        GetComponent<Collider2D>()->SetIsColliding(true);

        for (auto c : components)
            if (c->IsEnabled())
                c->OnCollisionEnter2D(other);
    }

    void GameObject::OnCollisionStay2D(const GameObject* other)
    {
        // go down scripts and call OnCollisionStay for all of them.

        for (auto c : components)
            if (c->IsEnabled())
                c->OnCollisionStay2D(other);
    }

    void GameObject::OnCollisionExit2D(const GameObject* other)
    {
        // go down scripts and call OnCollisionExit for all of them.
        GetComponent<Collider2D>()->SetIsColliding(false);

        for (auto c : components)
            if (c->IsEnabled())
                c->OnCollisionExit2D(other);
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
        tags.insert(tag);
    }

    void GameObject::RemoveTag(const std::string& tagName)
    {
        Tag tag;
        if (!TagRegistry::TryGet(tagName, tag))
            return;
        tags.erase(tag);
    }

    bool GameObject::HasTag(const std::string& tagName) const
    {
        Tag tag;
        if (!TagRegistry::TryGet(tagName, tag))
            return false;
        return tags.find(tag) != tags.end();
    }

    Tag TagRegistry::GetOrCreate(const std::string& name)
    {
        auto it = nameToTag.find(name);
        if (it != nameToTag.end())
            return it->second;

        Tag tag{nextId++};
        nameToTag.emplace(name, tag);
        tagToName.push_back(name);
        return tag;
    }

    bool TagRegistry::TryGet(const std::string& name, Tag& out)
    {
        auto it = nameToTag.find(name);
        if (it == nameToTag.end())
            return false;
        out = it->second;
        return true;
    }

    const std::string& TagRegistry::GetName(Tag tag)
    {
        return tagToName[tag.id];
    }

    bool TagRegistry::Rename(Tag tag, const std::string& newName)
    {
        auto taken = nameToTag.find(newName);
        if (taken != nameToTag.end())
        {
            return taken->second == tag;
        }

        auto oldName = tagToName[tag.id];
        auto itOld = nameToTag.find(oldName);
        if (itOld != nameToTag.end())
            nameToTag.erase(itOld);

        tagToName[tag.id] = newName;
        nameToTag.emplace(newName, tag);
        return true;
    }
}  // namespace mts
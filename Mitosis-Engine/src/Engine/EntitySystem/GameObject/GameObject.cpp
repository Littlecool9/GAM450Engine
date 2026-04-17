/**
 * @file GameObject.cpp
 * @author sumin.park
 * @brief  GameObject
 * @version 0.1
 * @date 1/8/2026 7:30:10 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "GameObject.h"
#include "../Component/Component.h"
#include "../Component/Transform.h"
#include "../Component/ComponentFactory.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/Scripting/NativeScriptComponent.h"
#include "Engine/SceneSystem/SceneSystem.h"

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
        if (jsonObj.Has("IsActive"))
        {
            SetActive(jsonObj["IsActive"].GetBool(true));
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
        json.WriteBool("IsActive", isActive);

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

#define FOR_CONDITION_COMPONENTS(cond, action)                 \
    for (size_t _i = 0, _n = components.size(); _i < _n; ++_i) \
        if (auto* c = components[_i]; c->cond)                 \
    action

    void GameObject::Init()
    {
        if (m_initialized)
            return;
        m_initialized = true;
        // Rebuild script cache before Init so scripts can query siblings
        m_scripts.clear();
        for (auto* c : components)
            if (auto* nsc = dynamic_cast<NativeScriptComponent*>(c))
                m_scripts.push_back(nsc);

        FOR_CONDITION_COMPONENTS(IsEnabled(), c->Init());
    }

    void GameObject::Awake()
    {
        for (auto* nsc : m_scripts)
            nsc->Awake();
    }

    void GameObject::OnEnable()
    {
        for (auto* c : components)
            if (c->IsEnabled())
                c->OnEnable();
    }

    void GameObject::FireLiveComponentLifecycle(Component* c)
    {
        c->Init();
        if (SceneSystem::IsInPlayMode() && isActive)
        {
            c->OnEnable();
            if (auto* nsc = dynamic_cast<NativeScriptComponent*>(c))
            {
                nsc->Awake();
                nsc->Start();  // direct call — new component, never started
            }
        }
    }

    void GameObject::Start()
    {
        if (!SceneSystem::IsInPlayMode() || m_startFired)
            return;
        m_startFired = true;
        for (auto* nsc : m_scripts)
            nsc->Start();
    }

    NativeScriptComponent* GameObject::AddNativeScript(const std::string& className)
    {
        if (!className.empty())
        {
            for (auto* nsc : m_scripts)
                if (nsc->GetClassName() == className)
                {
                    LOG_WARN("GameObject {} already has script '{}'", name, className);
                    break;
                }
        }

        auto* nsc = new NativeScriptComponent();
        nsc->owner = this;
        nsc->SetClassName(className);
        components.emplace_back(nsc);
        m_scripts.push_back(nsc);
        if (m_initialized)
            FireLiveComponentLifecycle(nsc);
        return nsc;
    }

    void GameObject::RemoveComponent(Component* c)
    {
        auto it = std::find(components.begin(), components.end(), c);
        if (it == components.end())
            return;

        if (auto* nsc = dynamic_cast<NativeScriptComponent*>(c))
        {
            auto sit = std::find(m_scripts.begin(), m_scripts.end(), nsc);
            if (sit != m_scripts.end())
                m_scripts.erase(sit);
        }

        delete *it;
        components.erase(it);
    }

    void GameObject::Update(float dt)
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->Update(dt));
    }

    void GameObject::LateUpdate(float dt)
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->LateUpdate(dt));
    }

    void GameObject::FixedUpdate(float dt)
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->FixedUpdate(dt));
    }

    void GameObject::Render()
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->Render());
    }

    void GameObject::DebugRender()
    {
        FOR_CONDITION_COMPONENTS(IsEnabled(), c->DebugRender());
    }

    void GameObject::SetActive(bool active)
    {
        if (isActive == active)
            return;
        isActive = active;
        if (isActive)
        {
            OnEnable();
            Start();  // no-op if not in play mode or already fired
        }
        else
        {
            for (auto* c : components)
                if (c->IsEnabled())
                    c->OnDisable();
        }
    }

    void GameObject::Destroy()
    {
        isDead = true;

        // on destroy called first in case scripts have to do something with components
        for (auto* nsc : m_scripts)
        {
            nsc->OnDestroy();
        }

        for (auto* c : components)
            if (c->IsEnabled())
                c->OnDisable();
    }

    void GameObject::OnCollisionEnter2D(const Collision2D& collision)
    {
        collision.self->SetIsColliding(true);
        // go down scripts and call OnCollisionEnter for all of them.

        for (auto c : components)
            if (c->IsEnabled())
                c->OnCollisionEnter2D(collision);
    }

    void GameObject::OnCollisionStay2D(const Collision2D& collision)
    {
        // go down scripts and call OnCollisionStay for all of them.

        for (auto c : components)
            if (c->IsEnabled())
                c->OnCollisionStay2D(collision);
    }

    void GameObject::OnCollisionExit2D(const Collision2D& collision)
    {
        collision.self->SetIsColliding(false);
        // go down scripts and call OnCollisionExit for all of them.

        for (auto c : components)
            if (c->IsEnabled())
                c->OnCollisionExit2D(collision);
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
    bool Tag::operator==(const std::string& name) const
    {
        return TagRegistry::GetName(*this) == name;
    }

    bool Tag::operator!=(const std::string& name) const
    {
        return TagRegistry::GetName(*this) != name;
    }

    bool Tag::operator==(const char* name) const
    {
        return TagRegistry::GetName(*this) == name;
    }

    bool Tag::operator!=(const char* name) const
    {
        return TagRegistry::GetName(*this) != name;
    }
}  // namespace mts
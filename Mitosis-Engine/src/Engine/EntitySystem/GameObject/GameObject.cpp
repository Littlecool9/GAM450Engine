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
    GameObject::GameObject()
    {
        AddComponent<Transform>();
    }
    GameObject::GameObject(ComponentFactory* obj)
    {
        // AddComponent<Transform>();
        componentFactory = obj;
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

        if (componentFactory == nullptr)
        {
            return;
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


            auto comp = componentFactory->Create(typeName, this);
            if (!comp)
                continue;

            // "Data" is optional depending on component
            if (item.Has("Data"))
            {
                comp->Deserialize(item["Data"]);
            }
            components.push_back(comp);
        }
    }

    void GameObject::Serialize(JsonDocument& json) const
    {
        // If you have name/id/etc
        // doc.WriteString("Name", name.c_str()); //or use writeint/writefloat if it's an id, or use string and do logic to read later 
        json.BeginArray("Components");  // "Components": [ ... ]
        for (auto c : components)
        {
            json.BeginObject();  // {
            c->Serialize(json);
            json.EndObject();  // }
        }
        json.EndArray();   // end "Components"

    }

    void GameObject::Init()
    {
        for (auto c : components)
            c->Init();
    }

    void GameObject::Update(float dt)
    {
        for (auto c : components)
            c->Update(dt);
    }

    void GameObject::LateUpdate(float dt)
    {
        for (auto c : components)
            c->LateUpdate(dt);
    }

    void GameObject::Render()
    {
        for (auto c : components)
            c->Render();
    }

}  // namespace mts
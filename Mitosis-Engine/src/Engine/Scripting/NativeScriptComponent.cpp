/**
 * @file NativeScriptComponent.cpp
 * @author rahul
 * @brief Component that hosts and drives a user-defined native C++ script attached to a GameObject.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "NativeScriptComponent.h"

#include "ScriptBase.h"
#include "ScriptFactory.h"
#include "Engine/Physics2D/Collision2D.h"

namespace mts
{
    NativeScriptComponent::NativeScriptComponent() : Component()
    {
    }

    NativeScriptComponent::~NativeScriptComponent()
    {
        if (m_delegateWired)
        {
            onEnable.Unsubscribe(m_onEnableId);
            onDisable.Unsubscribe(m_onDisableId);
        }
    }

    void NativeScriptComponent::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("Class"))
        {
            const char* cls = jsonObj["Class"].GetString("");
            if (cls && cls[0] != '\0')
                m_className = cls;
        }

        m_hasFields = false;
        m_fieldsJson.clear();

        if (jsonObj.Has("Fields") && jsonObj["Fields"].IsObject())
        {
            // Store it as owned string
            m_fieldsJson = jsonObj["Fields"].ToJsonString();
            m_hasFields = true;
        }
    }

    void NativeScriptComponent::Serialize(JsonDocument& jsonDoc) const
    {
        jsonDoc.WriteString("Type", "NativeScript");

        jsonDoc.BeginObject("Data");
        jsonDoc.WriteString("Class", m_className.c_str());

        if (m_hasFields & !m_fieldsJson.empty())
        {
            const bool ok = jsonDoc.WriteRawJsonObject("Fields", m_fieldsJson);
            if (!ok)
            {
                jsonDoc.WriteString("FieldsJson", m_fieldsJson.c_str());
            }
        }

        jsonDoc.EndObject();
    }

    void NativeScriptComponent::Init()
    {
        EnsureCreated();

        if (!m_delegateWired)
        {
            m_onEnableId = onEnable.Subscribe(
                [this]()
                {
                    if (m_script)
                        m_script->OnEnable();
                });

            m_onDisableId = onDisable.Subscribe(
                [this]()
                {
                    if (m_script)
                        m_script->OnDisable();
                });

            m_delegateWired = true;
        }
    }
    void NativeScriptComponent::EnsureCreated()
    {
        if (m_created)
            return;

        m_created = true;

        if (m_className.empty())
        {
            LOG_WARN("No Class specified.");
            return;
        }

        ScriptBase* raw = ScriptFactory::Create(m_className);
        if (!raw)
        {
            LOG_ERROR("Script '{}' not registered.", m_className);
            return;
        }

        m_script.reset(raw);
        m_script->SetOwner(GetGameObject());

        // Parse fields into an owned document so JsonValue views are valid
        if (m_hasFields)
        {
            if (!m_fieldsDoc.LoadFromString(m_fieldsJson))
            {
                LOG_WARN("Failed to parse Fields JSON for '{}'", m_className);
            }
            else
            {
                // Root is the fields object
                m_script->DeserializeFields(m_fieldsDoc.Root());
            }
        }

        m_script->OnCreate();

        if (IsEnabled())
            m_script->OnEnable();
    }

    void NativeScriptComponent::Update(float dt)
    {
        if (!IsEnabled())
            return;

        EnsureCreated();

        if (m_script)
            m_script->OnUpdate(dt);
    }

    void NativeScriptComponent::LateUpdate(float dt)
    {
        if (!IsEnabled())
            return;

        if (m_script)
            m_script->OnLateUpdate(dt);
    }

    void NativeScriptComponent::FixedUpdate(float dt)
    {
        if (m_script)
            m_script->OnFixedUpdate(dt);
    }

    void NativeScriptComponent::DebugRender()
    {
        // no-op
    }

    void NativeScriptComponent::OnCollisionEnter2D(const mts::Collision2D& collision)
    {
        if (!IsEnabled())
            return;

        EnsureCreated();

        if (m_script)
            m_script->OnCollisionEnter2D(collision);
    }

    void NativeScriptComponent::OnCollisionStay2D(const mts::Collision2D& collision)
    {
        if (!IsEnabled())
            return;

        if (m_script)
            m_script->OnCollisionStay2D(collision);
    }

    void NativeScriptComponent::OnCollisionExit2D(const mts::Collision2D& collision)
    {
        if (!IsEnabled())
            return;

        if (m_script)
            m_script->OnCollisionExit2D(collision);
    }

    const std::string& NativeScriptComponent::GetClassName() const
    {
        return m_className;
    }

    void NativeScriptComponent::SyncFieldsToJson()
    {
        if (!m_script)
            return;
        JsonDocument out;
        m_script->SerializeFields(out);
        out.SaveToString(m_fieldsJson);
        m_hasFields = !m_fieldsJson.empty();
    }

    Component* NativeScriptComponent::Clone(GameObject* newOwner) const
    {
        NativeScriptComponent* c = new NativeScriptComponent();
        c->InitFromClone(newOwner);
        c->m_className = m_className;
        c->m_fieldsJson = m_fieldsJson;
        c->m_hasFields = m_hasFields;
        return c;
    }
}  // namespace mts
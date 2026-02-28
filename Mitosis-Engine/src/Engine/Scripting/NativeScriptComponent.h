/**
 * @file NativeScriptComponent.h
 * @author rahul
 * @brief Component that hosts and drives a user-defined native C++ script attached to a GameObject.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include "Engine/EntitySystem/Component/Component.h"

namespace mts
{
    class ScriptBase;
    class JsonValue;

    class NativeScriptComponent : public Component
    {
       public:
        NativeScriptComponent();
        ~NativeScriptComponent() override;

        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void DebugRender() override;

        Component* Clone(GameObject* newOwner) const override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& jsonDoc) const override;

        virtual void OnCollisionEnter2D(const Collision2D& collision) override;
        virtual void OnCollisionStay2D(const Collision2D& collision) override;
        virtual void OnCollisionExit2D(const Collision2D& collision) override;

        const std::string& GetClassName() const;
        void SetClassName(const std::string& name) { m_className = name; }
        bool HasScript() const { return m_script != nullptr; }
        ScriptBase* GetScript() const { return m_script.get(); }

        void SyncFieldsToJson();

       private:
        void EnsureCreated();

        std::string m_className;

        // Own the fields as a string so it survives past scene document lifetime
        std::string m_fieldsJson;

        // Owned document parsed from m_fieldsJson during EnsureCreated()
        JsonDocument m_fieldsDoc;
        bool m_hasFields = false;

        std::unique_ptr<ScriptBase> m_script;
        bool m_created = false;

        Delegate<>::Id m_onEnableId = 0;
        Delegate<>::Id m_onDisableId = 0;
        bool m_delegateWired = false;
    };

}  // namespace mts
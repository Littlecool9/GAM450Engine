/**
 * @file NativeScriptDrawer.cpp
 * @author sumin.park
 * @brief  drawer for native scripts
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "NativeScriptDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Scripting/NativeScriptComponent.h"
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    void NativeScriptDrawer::Draw(GameObject* go)
    {
        auto* nsc = go->GetComponent<NativeScriptComponent>();
        if (!nsc)
            return;

        ImGui::PushID("NativeScriptDrawer");

        if (ImGui::CollapsingHeader("Native Script", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Class: %s", nsc->GetClassName().c_str());

            ScriptBase* script = nsc->GetScript();
            if (!script)
            {
                ImGui::TextDisabled("(not initialized)");
            }
            else
            {
                std::vector<ScriptField> fields;
                script->GetInspectorFields(fields);

                bool changed = false;
                for (auto& field : fields)
                {
                    switch (field.type)
                    {
                        case ScriptField::Type::Float:
                            if (ImGui::DragFloat(field.name, field.ptr.f, field.step))
                                changed = true;
                            break;
                        case ScriptField::Type::Int:
                            if (ImGui::DragInt(field.name, field.ptr.i))
                                changed = true;
                            break;
                        case ScriptField::Type::Bool:
                            if (ImGui::Checkbox(field.name, field.ptr.b))
                                changed = true;
                            break;
                    }
                }

                if (changed)
                    nsc->SyncFieldsToJson();
            }
        }

        ImGui::PopID();
    }
}  // namespace mts

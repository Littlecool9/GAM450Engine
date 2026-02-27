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
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/UUID/UUID.h"

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
                        case ScriptField::Type::GameObject:
                        {
                            Scene* scene = SceneSystem::GetCurrentScene();
                            std::string goName = (*field.ptr.go) ? (*field.ptr.go)->GetName() : "None";
                            ImGui::Text("%s:", field.name);
                            ImGui::SameLine();
                            std::string popupId = std::string("##gopick_") + field.name;
                            if (ImGui::Button(goName.c_str()))
                                ImGui::OpenPopup(popupId.c_str());
                            if (ImGui::BeginDragDropTarget())
                            {
                                if (auto* p = ImGui::AcceptDragDropPayload("GAMEOBJECT_UUID"))
                                {
                                    UUID id = UUID::FromString(static_cast<const char*>(p->Data));
                                    if (scene)
                                    {
                                        *field.ptr.go = scene->FindGameObjectByUUID(id);
                                        changed = true;
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }
                            if (ImGui::BeginPopup(popupId.c_str()) && scene)
                            {
                                for (GameObject* c : scene->GetGameObjects())
                                {
                                    if (!c || c->IsDead())
                                        continue;
                                    if (ImGui::Selectable(c->GetName().c_str()))
                                    {
                                        *field.ptr.go = c;
                                        changed = true;
                                    }
                                }
                                ImGui::EndPopup();
                            }
                            break;
                        }
                    }
                }

                if (changed)
                    nsc->SyncFieldsToJson();
            }
        }

        ImGui::PopID();
    }
}  // namespace mts

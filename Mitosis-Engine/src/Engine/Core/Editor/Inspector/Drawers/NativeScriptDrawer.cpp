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
#include <imgui_stdlib.h>

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Scripting/NativeScriptComponent.h"
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/UUID/UUID.h"

namespace mts
{
    bool NativeScriptDrawer::Draw(GameObject* go)
    {
        auto nscs = go->GetComponents<NativeScriptComponent>();
        if (nscs.empty())
            return false;

        bool anyDrawn = false;
        int idx = 0;
        for (auto* nsc : nscs)
        {
            std::string header = "Native Script: " + nsc->GetClassName();
            ImGui::PushID(idx++);

            bool open = ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
            bool removed = false;
            if (ImGui::BeginPopupContextItem("##remove_ctx"))
            {
                if (ImGui::MenuItem("Remove Component"))
                {
                    go->RemoveComponent(nsc);
                    removed = true;
                }
                ImGui::EndPopup();
            }
            if (open && !removed)
            {
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
                        ImGui::PushID(field.name);
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
                            case ScriptField::Type::Vec2:
                                if (ImGui::DragFloat2(field.name, &field.ptr.v2->x, field.step))
                                    changed = true;
                                break;
                            case ScriptField::Type::Vec3:
                                if (ImGui::DragFloat3(field.name, &field.ptr.v3->x, field.step))
                                    changed = true;
                                break;
                            case ScriptField::Type::Color:
                                if (ImGui::ColorEdit3(field.name, &field.ptr.v3->x))
                                    changed = true;
                                break;
                            case ScriptField::Type::String:
                            {
                                std::string& str = *field.ptr.s;
                                if (ImGui::InputText(field.name, &str, ImGuiInputTextFlags_None))
                                    changed = true;
                            }
                            break;

                            case ScriptField::Type::GameObject:
                            {
                                Scene* scene = SceneSystem::GetCurrentScene();
                                // Lazy-resolve from UUID after a scene reload
                                if (*field.ptr.go == nullptr && field.uuid &&
                                    field.uuid->IsValid() && scene)
                                    *field.ptr.go = scene->FindGameObjectByUUID(*field.uuid);
                                std::string goName =
                                    (*field.ptr.go) ? (*field.ptr.go)->GetName() : "None";
                                ImGui::Text("%s:", field.name);
                                ImGui::SameLine();
                                std::string popupId = std::string("##gopick_") + field.name;
                                if (ImGui::Button(goName.c_str()))
                                    ImGui::OpenPopup(popupId.c_str());
                                if (ImGui::BeginDragDropTarget())
                                {
                                    if (auto* p = ImGui::AcceptDragDropPayload("GAMEOBJECT_UUID"))
                                    {
                                        UUID id =
                                            UUID::FromString(static_cast<const char*>(p->Data));
                                        if (scene)
                                        {
                                            *field.ptr.go = scene->FindGameObjectByUUID(id);
                                            if (field.uuid)
                                                *field.uuid = id;
                                            changed = true;
                                        }
                                    }
                                    ImGui::EndDragDropTarget();
                                }
                                if (ImGui::BeginPopup(popupId.c_str()) && scene)
                                {
                                    int i = 0;
                                    for (GameObject* c : scene->GetGameObjects())
                                    {
                                        if (!c || c->IsDead())
                                            continue;

                                        std::string label =
                                            c->GetName().empty() ? "Unnamed" : c->GetName();
                                        label += "##";
                                        label += std::to_string(i++);

                                        if (ImGui::Selectable(label.c_str()))
                                        {
                                            *field.ptr.go = c;
                                            if (field.uuid)
                                                *field.uuid = c->GetId();
                                            changed = true;
                                        }
                                    }
                                    ImGui::EndPopup();
                                }
                                break;
                            }
                            case ScriptField::Type::Component:
                            {
                                Scene* scene = SceneSystem::GetCurrentScene();
                                // Lazy-resolve from UUID after a scene reload
                                if (*field.ptr.comp == nullptr && field.uuid &&
                                    field.uuid->IsValid() && scene && field.resolver)
                                {
                                    if (GameObject* target =
                                            scene->FindGameObjectByUUID(*field.uuid))
                                        *field.ptr.comp = field.resolver(target);
                                }
                                std::string label = "None";
                                if (field.uuid && field.uuid->IsValid() && scene)
                                {
                                    if (GameObject* target =
                                            scene->FindGameObjectByUUID(*field.uuid))
                                        label = target->GetName();
                                }
                                ImGui::Text("%s:", field.name);
                                ImGui::SameLine();
                                ImGui::Button(label.c_str());
                                if (ImGui::BeginDragDropTarget())
                                {
                                    UUID id = UUID::GenerateNil();
                                    if (auto* p = ImGui::AcceptDragDropPayload("GAMEOBJECT_UUID"))
                                        id = UUID::FromString(static_cast<const char*>(p->Data));
                                    else if (auto* p =
                                                 ImGui::AcceptDragDropPayload("COMPONENT_GO_UUID"))
                                        id = UUID::FromString(static_cast<const char*>(p->Data));
                                    if (id.IsValid() && scene)
                                    {
                                        if (field.uuid)
                                            *field.uuid = id;
                                        if (field.resolver)
                                        {
                                            if (GameObject* target =
                                                    scene->FindGameObjectByUUID(id))
                                                *field.ptr.comp = field.resolver(target);
                                        }
                                        changed = true;
                                    }
                                    ImGui::EndDragDropTarget();
                                }
                                break;
                            }
                        }
                        ImGui::PopID();
                    }

                    if (changed)
                        nsc->SyncFieldsToJson();
                }
            }

            ImGui::PopID();
            anyDrawn = true;
        }
        return anyDrawn;
    }
}  // namespace mts

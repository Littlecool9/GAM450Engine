/**
 * @file AnimationSystemDrawer.cpp
 * @author sumin.park
 * @brief drawer for AnimationSystem component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "AnimationSystemDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Resource/ResourceManager.h"

namespace mts
{
    static const char* StateToStr(AnimationSystem::States s)
    {
        using S = AnimationSystem::States;
        switch (s)
        {
            case S::Idle:
                return "Idle";
            case S::Walking:
                return "Walking";
            case S::Jump:
                return "Jump";
            case S::Falling:
                return "Falling";
            case S::Attack1:
                return "Attack1";
            case S::Attack2:
                return "Attack2";
            default:
                return "Unknown";
        }
    }

    bool AnimationSystemDrawer::Draw(GameObject* go)
    {
        auto* as = go->GetComponent<AnimationSystem>();
        if (!as)
            return false;

        ImGui::PushID("AnimationSystemDrawer");

        bool open = ImGui::CollapsingHeader("AnimationSystem", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(as);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            ImGui::LabelText("Current State", "%s", StateToStr(as->GetCurrentState()));

            constexpr AnimationSystem::States allStates[] = {
                AnimationSystem::States::Idle,
                AnimationSystem::States::Walking,
                AnimationSystem::States::Jump,
                AnimationSystem::States::Falling,
                AnimationSystem::States::Attack1,
                AnimationSystem::States::Attack2,
            };

            for (auto state : allStates)
            {
                const char* stateName = StateToStr(state);
                auto it = as->dictionary.find(state);

                ImGui::PushID(static_cast<int>(state));

                if (ImGui::TreeNode(stateName))
                {
                    if (it == as->dictionary.end())
                    {
                        ImGui::TextDisabled("(not configured)");
                        if (ImGui::Button("Add Entry"))
                            as->dictionary[state] = AnimationSystem::Info{};
                    }
                    else
                    {
                        auto& info = it->second;

                        // Mesh
                        char meshBuf[256];
                        snprintf(meshBuf, sizeof(meshBuf), "%s", info.meshPath.c_str());
                        ImGui::InputText("Mesh",
                                         meshBuf,
                                         sizeof(meshBuf),
                                         ImGuiInputTextFlags_ReadOnly);
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                            {
                                info.meshPath = static_cast<const char*>(p->Data);
                                info.mMesh = ResourceManager::GetMesh(info.meshPath.c_str());
                            }
                            ImGui::EndDragDropTarget();
                        }

                        // Shader
                        char shaderBuf[256];
                        snprintf(shaderBuf, sizeof(shaderBuf), "%s", info.shaderPath.c_str());
                        ImGui::InputText("Shader",
                                         shaderBuf,
                                         sizeof(shaderBuf),
                                         ImGuiInputTextFlags_ReadOnly);
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                            {
                                info.shaderPath = static_cast<const char*>(p->Data);
                                info.mShader = ResourceManager::GetShader(info.shaderPath.c_str());
                            }
                            ImGui::EndDragDropTarget();
                        }

                        ImGui::Separator();

                        ImGui::DragInt("Frame Count", &info.frameCount, 1, 0, 512);
                        ImGui::DragFloat("Frame Duration", &info.frameDuration, 0.001f, 0.0f, 5.0f);
                        ImGui::Checkbox("Looping", &info.isLooping);
                        ImGui::Checkbox("Animation Enabled", &info.animation);

                        bool isRunning = info.isRunning;
                        int frameIndex = info.frameIndex;
                        ImGui::BeginDisabled(true);
                        ImGui::DragInt("Frame Index", &frameIndex);
                        ImGui::Checkbox("Is Running", &isRunning);
                        ImGui::EndDisabled();

                        ImGui::Separator();

                        if (ImGui::Button("Set State"))
                            as->ChangeState(state);
                    }

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts

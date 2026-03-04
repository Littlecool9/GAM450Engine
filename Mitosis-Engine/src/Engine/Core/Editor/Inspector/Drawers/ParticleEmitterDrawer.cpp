/**
 * @file ParticleEmitterDrawer.cpp
 * @author sumin.park
 * @brief drawer for ParticleEmitter component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ParticleEmitterDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Particles/ParticleEmitter.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"

namespace mts
{
    bool ParticleEmitterDrawer::Draw(GameObject* go)
    {
        auto* pe = go->GetComponent<ParticleEmitter>();
        if (!pe)
            return false;

        ImGui::PushID("ParticleEmitterDrawer");

        bool open = ImGui::CollapsingHeader("ParticleEmitter", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(pe);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            // --- Rendering ---
            {
                const std::string& cur =
                    pe->GetTexture() ? pe->GetTexture()->GetPath() : std::string{};
                char buf[256];
                snprintf(buf, sizeof(buf), "%s", cur.c_str());
                ImGui::InputText("Texture##pe", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::BeginDragDropTarget())
                {
                    if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                        pe->SetTexture(
                            ResourceManager::GetTexture(static_cast<const char*>(p->Data)));
                    ImGui::EndDragDropTarget();
                }
            }
            {
                const std::string& cur = pe->GetMesh() ? pe->GetMesh()->GetPath() : std::string{};
                char buf[256];
                snprintf(buf, sizeof(buf), "%s", cur.c_str());
                ImGui::InputText("Mesh##pe", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::BeginDragDropTarget())
                {
                    if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                        pe->SetMesh(ResourceManager::GetMesh(static_cast<const char*>(p->Data)));
                    ImGui::EndDragDropTarget();
                }
            }
            {
                const std::string& cur =
                    pe->GetShader() ? pe->GetShader()->GetPath() : std::string{};
                char buf[256];
                snprintf(buf, sizeof(buf), "%s", cur.c_str());
                ImGui::InputText("Shader##pe", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::BeginDragDropTarget())
                {
                    if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                        pe->SetShader(
                            ResourceManager::GetShader(static_cast<const char*>(p->Data)));
                    ImGui::EndDragDropTarget();
                }
            }

            ImGui::Separator();

            float alpha = pe->GetAlpha();
            if (ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f))
                pe->SetAlpha(alpha);

            glm::vec3 tint = pe->GetTintColor();
            if (ImGui::ColorEdit3("Tint", &tint.x))
                pe->SetTintColor(tint);

            int rows = pe->GetNumRows();
            if (ImGui::DragInt("Rows", &rows, 1, 1, 512))
                pe->SetNumRows(rows);

            int cols = pe->GetNumCols();
            if (ImGui::DragInt("Cols", &cols, 1, 1, 512))
                pe->SetNumCols(cols);

            ImGui::Separator();

            // --- Emission ---
            glm::vec2 emitterScale = pe->GetEmitterScale();
            if (ImGui::DragFloat2("Emitter Scale", &emitterScale.x, 0.05f))
                pe->SetEmitterScale(emitterScale);

            float emitRate = pe->GetEmitRate();
            if (ImGui::DragFloat("Emit Rate", &emitRate, 0.1f, 0.0f, 1000.0f))
                pe->SetEmitRate(emitRate);

            int emitMax = static_cast<int>(pe->GetEmitMax());
            if (ImGui::DragInt("Emit Max", &emitMax, 1, 0, 10000))
                pe->SetEmitMax(static_cast<unsigned>(emitMax));

            ImGui::Separator();

            // --- Particle properties ---
            glm::vec2 scaleMin = pe->GetScaleMin();
            if (ImGui::DragFloat2("Scale Min", &scaleMin.x, 0.01f, 0.0f, 100.0f))
                pe->SetScaleMin(scaleMin);

            glm::vec2 scaleMax = pe->GetScaleMax();
            if (ImGui::DragFloat2("Scale Max", &scaleMax.x, 0.01f, 0.0f, 100.0f))
                pe->SetScaleMax(scaleMax);

            glm::vec2 accel = pe->GetAcceleration();
            if (ImGui::DragFloat2("Acceleration", &accel.x, 0.1f))
                pe->SetAcceleration(accel);

            glm::vec2 dirRange = pe->GetDirectionRange();
            if (ImGui::DragFloat2("Direction Range", &dirRange.x, 0.01f))
                pe->SetDirectionRange(dirRange);

            float lifetimeMin = pe->GetLifetimeMin();
            if (ImGui::DragFloat("Lifetime Min", &lifetimeMin, 0.01f, 0.0f, 60.0f))
                pe->SetLifetimeMin(lifetimeMin);

            float lifetimeMax = pe->GetLifetimeMax();
            if (ImGui::DragFloat("Lifetime Max", &lifetimeMax, 0.01f, 0.0f, 60.0f))
                pe->SetLifetimeMax(lifetimeMax);

            float speedMin = pe->GetSpeedMin();
            if (ImGui::DragFloat("Speed Min", &speedMin, 0.1f, 0.0f, 10000.0f))
                pe->SetSpeedMin(speedMin);

            float speedMax = pe->GetSpeedMax();
            if (ImGui::DragFloat("Speed Max", &speedMax, 0.1f, 0.0f, 10000.0f))
                pe->SetSpeedMax(speedMax);

            float rotSpeedMin = pe->GetRotationSpeedMin();
            if (ImGui::DragFloat("Rot Speed Min", &rotSpeedMin, 0.01f))
                pe->SetRotationSpeedMin(rotSpeedMin);

            float rotSpeedMax = pe->GetRotationSpeedMax();
            if (ImGui::DragFloat("Rot Speed Max", &rotSpeedMax, 0.01f))
                pe->SetRotationSpeedMax(rotSpeedMax);

            float rotMin = pe->GetRotationMin();
            if (ImGui::DragFloat("Rotation Min", &rotMin, 0.01f))
                pe->SetRotationMin(rotMin);

            float rotMax = pe->GetRotationMax();
            if (ImGui::DragFloat("Rotation Max", &rotMax, 0.01f))
                pe->SetRotationMax(rotMax);

            ImGui::Separator();

            bool oneShot = pe->GetOneShot();
            if (ImGui::Checkbox("One Shot", &oneShot))
                pe->SetOneShot(oneShot);

            bool isRunning = pe->GetIsRunning();
            ImGui::BeginDisabled(true);
            ImGui::Checkbox("Is Running", &isRunning);
            ImGui::EndDisabled();

            if (ImGui::Button("Toggle Emitter"))
                pe->ToggleEmitter();
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts

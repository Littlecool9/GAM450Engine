/**
 * @file SpriteRendererDrawer.cpp
 * @author sumin.park
 * @brief  drawer for sprite renderer
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SpriteRendererDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Resource/ResourceManager.h"

namespace mts
{
    bool SpriteRendererDrawer::Draw(mts::GameObject* go)
    {
        auto* sr = go->GetComponent<SpriteRenderer>();
        if (!sr)
            return false;

        ImGui::PushID("SpriteRendererDrawer");

        if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Texture
            {
                const std::string& cur =
                    sr->GetTexture() ? sr->GetTexture()->GetPath() : std::string{};
                char buf[256];
                snprintf(buf, sizeof(buf), "%s", cur.c_str());
                ImGui::InputText("Texture##path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::BeginDragDropTarget())
                {
                    if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                        sr->SetTexture(
                            ResourceManager::GetTexture(static_cast<const char*>(p->Data)));
                    ImGui::EndDragDropTarget();
                }
            }

            // Mesh
            {
                const std::string& cur = sr->GetMesh() ? sr->GetMesh()->GetPath() : std::string{};
                char buf[256];
                snprintf(buf, sizeof(buf), "%s", cur.c_str());
                ImGui::InputText("Mesh##path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::BeginDragDropTarget())
                {
                    if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                        sr->SetMesh(ResourceManager::GetMesh(static_cast<const char*>(p->Data)));
                    ImGui::EndDragDropTarget();
                }
            }

            // Shader
            {
                const std::string& cur =
                    sr->GetShader() ? sr->GetShader()->GetPath() : std::string{};
                char buf[256];
                snprintf(buf, sizeof(buf), "%s", cur.c_str());
                ImGui::InputText("Shader##path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::BeginDragDropTarget())
                {
                    if (auto* p = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                        sr->SetShader(
                            ResourceManager::GetShader(static_cast<const char*>(p->Data)));
                    ImGui::EndDragDropTarget();
                }
            }

            ImGui::Separator();

            int rows = sr->GetRows();
            if (ImGui::DragInt("Rows", &rows, 1.0f, 1, 512))
                sr->SetRows(rows);

            int cols = sr->GetCols();
            if (ImGui::DragInt("Cols", &cols, 1.0f, 1, 512))
                sr->SetCols(cols);

            int frame = sr->GetFrame();
            int maxFrame = std::max(0, rows * cols - 1);
            if (ImGui::SliderInt("Frame", &frame, 0, maxFrame))
                sr->SetFrame(frame);

            float alpha = sr->GetAlpha();
            if (ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f))
                sr->SetAlpha(alpha);

            glm::vec3 tint = sr->GetTintColor();
            if (ImGui::ColorEdit3("Tint", &tint.x))
                sr->SetTintColor(tint);

            bool flipX = sr->GetFlipX();
            if (ImGui::Checkbox("Flip X", &flipX))
                sr->SetFlipX(flipX);

            bool flipY = sr->GetFlipY();
            if (ImGui::Checkbox("Flip Y", &flipY))
                sr->SetFlipY(flipY);

            bool hasAlpha = sr->HasTextureAlpha();
            if (ImGui::Checkbox("Texture Has Alpha", &hasAlpha))
                sr->SetHasTextureAlpha(hasAlpha);
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts
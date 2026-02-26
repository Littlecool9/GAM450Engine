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

namespace mts
{
    static const char* PtrLabel(const void* p)
    {
        static char buf[64];
        snprintf(buf, sizeof(buf), "%p", p);
        return buf;
    }

    void SpriteRendererDrawer::Draw(mts::GameObject* go)
    {
        auto* sr = go->GetComponent<SpriteRenderer>();
        if (!sr)
            return;

        ImGui::PushID("SpriteRendererDrawer");

        if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // display pointers for now
            // will replace with asset pickers.
            ImGui::Text("Mesh   : %s", PtrLabel(sr->GetMesh()));
            ImGui::Text("Shader : %s", PtrLabel(sr->GetShader()));
            ImGui::Text("Texture: %s", PtrLabel(sr->GetTexture()));

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
    }
}  // namespace mts
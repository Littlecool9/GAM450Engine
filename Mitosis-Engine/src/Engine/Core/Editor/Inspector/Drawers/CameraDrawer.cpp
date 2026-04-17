/**
 * @file CameraDrawer.cpp
 * @author sumin.park
 * @brief drawer for CameraComponent
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "CameraDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Camera/Camera.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Renderer/PostProcessEffect.h"
#include "Engine/Renderer/PostProcessStack.h"

namespace mts
{
    // Helper to draw UI for a single post-process effect
    static void DrawEffectUI(PostProcessEffect* effect, int index, PostProcessStack& stack)
    {
        ImGui::PushID(index);

        const char* typeName = effect->GetTypeName();
        bool open =
            ImGui::TreeNodeEx(typeName,
                              ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

        // Enable/disable + remove on the same line
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60.0f);
        ImGui::Checkbox("##en", &effect->enabled);
        ImGui::SameLine();
        bool remove = ImGui::SmallButton("X");

        if (open)
        {
            // Move up / move down
            if (index > 0 && ImGui::SmallButton("Up"))
                stack.MoveEffect(index, index - 1);
            if (index < static_cast<int>(stack.GetEffects().size()) - 1 &&
                ImGui::SmallButton("Down"))
                stack.MoveEffect(index, index + 1);

            // Effect-specific parameters
            if (auto* tint = dynamic_cast<ColorTintEffect*>(effect))
            {
                ImGui::ColorEdit3("Tint Color", &tint->color.x);
            }
            else if (auto* bcs = dynamic_cast<BCSEffect*>(effect))
            {
                ImGui::SliderFloat("Brightness", &bcs->brightness, -1.0f, 1.0f);
                ImGui::SliderFloat("Contrast", &bcs->contrast, 0.0f, 3.0f);
                ImGui::SliderFloat("Saturation", &bcs->saturation, 0.0f, 3.0f);
            }
            else if (auto* blur = dynamic_cast<GaussianBlurEffect*>(effect))
            {
                ImGui::SliderInt("Radius", &blur->radius, 1, 15);
                ImGui::SliderFloat("Sigma", &blur->sigma, 0.1f, 10.0f);
            }
            else if (auto* mb = dynamic_cast<MotionBlurEffect*>(effect))
            {
                ImGui::SliderFloat("Strength", &mb->strength, 0.0f, 0.95f);
            }
            else if (auto* custom = dynamic_cast<CustomShaderEffect*>(effect))
            {
                char buf[256]{};
                strncpy(buf, custom->shaderResource.c_str(), sizeof(buf) - 1);
                if (ImGui::InputText("Shader", buf, sizeof(buf)))
                    custom->shaderResource = buf;

                // Float params
                for (auto& p : custom->floatParams)
                {
                    ImGui::DragFloat(p.name.c_str(), &p.value, 0.01f);
                }
                // Vec3 params
                for (auto& p : custom->vec3Params)
                {
                    ImGui::DragFloat3(p.name.c_str(), &p.value.x, 0.01f);
                }
                // Int params
                for (auto& p : custom->intParams)
                {
                    ImGui::DragInt(p.name.c_str(), &p.value);
                }
            }

            ImGui::TreePop();
        }

        ImGui::PopID();

        if (remove)
            stack.RemoveEffect(index);
    }

    bool CameraDrawer::Draw(GameObject* go)
    {
        auto* cam = go->GetComponent<CameraComponent>();
        if (!cam)
            return false;

        ImGui::PushID("CameraDrawer");

        bool open = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(cam);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            ImGui::Checkbox("Active", &cam->isActive);
            ImGui::DragFloat("Ortho Size", &cam->orthoSize, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("Near Plane", &cam->nearPlane, 0.01f);
            ImGui::DragFloat("Far Plane", &cam->farPlane, 0.1f);

            // --- Post Processing ---
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Post Processing", ImGuiTreeNodeFlags_DefaultOpen))
            {
                PostProcessStack& stack = cam->GetPostProcessStack();
                auto& effects = stack.GetEffects();

                for (int i = 0; i < static_cast<int>(effects.size()); ++i)
                {
                    if (effects[i])
                        DrawEffectUI(effects[i].get(), i, stack);
                }

                // Add effect button
                if (ImGui::Button("Add Effect..."))
                    ImGui::OpenPopup("AddEffectPopup");

                if (ImGui::BeginPopup("AddEffectPopup"))
                {
                    if (ImGui::MenuItem("Color Tint"))
                        stack.AddEffect(CreatePostProcessEffect("ColorTint"));
                    if (ImGui::MenuItem("Brightness/Contrast/Saturation"))
                        stack.AddEffect(CreatePostProcessEffect("BrightnessContrastSaturation"));
                    if (ImGui::MenuItem("Gaussian Blur"))
                        stack.AddEffect(CreatePostProcessEffect("GaussianBlur"));
                    if (ImGui::MenuItem("Motion Blur"))
                        stack.AddEffect(CreatePostProcessEffect("MotionBlur"));
                    if (ImGui::MenuItem("Custom Shader"))
                        stack.AddEffect(CreatePostProcessEffect("CustomShader"));
                    ImGui::EndPopup();
                }
            }
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts

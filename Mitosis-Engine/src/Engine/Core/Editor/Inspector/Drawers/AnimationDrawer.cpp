/**
 * @file AnimationDrawer.cpp
 * @author sumin.park
 * @brief drawer for Animation component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "AnimationDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool AnimationDrawer::Draw(GameObject* go)
    {
        auto* anim = go->GetComponent<Animation>();
        if (!anim)
            return false;

        ImGui::PushID("AnimationDrawer");

        bool open = ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(anim);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            ImGui::LabelText("Frame Index", "%d", anim->GetFrameIndex());

            int startFrame = anim->GetStartFrame();
            if (ImGui::DragInt("Start Frame", &startFrame, 1, 0, 512))
                anim->SetStartFrame(startFrame);

            int frameCount = anim->GetFrameCount();
            if (ImGui::DragInt("Frame Count", &frameCount, 1, 0, 512))
                anim->SetFrameCount(frameCount);

            float frameDuration = anim->GetFrameDuration();
            if (ImGui::DragFloat("Frame Duration", &frameDuration, 0.001f, 0.0f, 5.0f))
                anim->SetFrameDuration(frameDuration);

            bool isLooping = anim->GetIsLooping();
            if (ImGui::Checkbox("Looping", &isLooping))
                anim->SetIsLooping(isLooping);

            bool isRunning = anim->GetIsRunning();
            bool isDone = anim->GetIsDone();
            ImGui::BeginDisabled(true);
            ImGui::Checkbox("Running", &isRunning);
            ImGui::SameLine();
            ImGui::Checkbox("Done", &isDone);
            ImGui::EndDisabled();

            ImGui::Separator();

            if (ImGui::Button("Stop"))
                anim->AnimationStop();
            if (ImGui::Button("Start"))
                anim->AnimationPlay(startFrame, frameCount, frameDuration, isLooping);
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts

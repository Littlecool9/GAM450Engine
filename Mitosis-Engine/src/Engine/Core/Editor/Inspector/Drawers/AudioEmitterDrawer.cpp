/**
 * @file AudioEmitterDrawer.cpp
 * @author sumin.park
 * @brief drawer for AudioEmitter component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AudioEmitterDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Audio/AudioEmitter.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool AudioEmitterDrawer::Draw(GameObject* go)
    {
        auto* ae = go->GetComponent<AudioEmitter>();
        if (!ae)
            return false;

        ImGui::PushID("AudioEmitterDrawer");

        bool open = ImGui::CollapsingHeader("AudioEmitter", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(ae);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            char eventBuf[256];
            snprintf(eventBuf, sizeof(eventBuf), "%s", ae->eventId.c_str());
            if (ImGui::InputText("Event ID", eventBuf, sizeof(eventBuf)))
                ae->eventId = eventBuf;

            ImGui::DragFloat2("Position Offset", &ae->mPositionOffset.x, 0.05f);
            ImGui::DragFloat("Start Volume", &ae->mStartVolume, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Start Pitch", &ae->mStartPitch, 0.01f, 0.1f, 3.0f);
            ImGui::DragFloat("Max Audible Distance",
                             &ae->mMaxAudibleDistance,
                             1.0f,
                             0.0f,
                             10000.0f);

            ImGui::Separator();

            ImGui::Checkbox("Play On Start", &ae->mPlayOnStart);
            ImGui::Checkbox("Spatialized 2D", &ae->mSpatialized2D);
            ImGui::Checkbox("One Shot", &ae->mIsOneShot);

            ImGui::Separator();

            ImGui::BeginDisabled(ae->isPlaying);
            if (ImGui::Button("Play"))
                ae->Play();
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(!ae->isPlaying);
            if (ImGui::Button("Stop"))
                ae->Stop();
            ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::TextUnformatted(ae->isPlaying ? "(Playing)" : "(Stopped)");
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts

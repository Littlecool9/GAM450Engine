/**
 * @file AudioListenerDrawer.cpp
 * @author sumin.park
 * @brief drawer for AudioListener component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "AudioListenerDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Audio/AudioListener.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool AudioListenerDrawer::Draw(GameObject* go)
    {
        auto* al = go->GetComponent<AudioListener>();
        if (!al)
            return false;

        ImGui::PushID("AudioListenerDrawer");

        bool open = ImGui::CollapsingHeader("AudioListener", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(al);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            ImGui::TextDisabled("(Listens for spatial audio)");
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts

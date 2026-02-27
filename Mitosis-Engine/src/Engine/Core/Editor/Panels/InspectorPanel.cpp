/**
 * @file InspectorPanel.cpp
 * @author sumin.park
 * @brief  inspector ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "InspectorPanel.h"

#include <imgui.h>

#include "Engine/Core/Editor/Inspector/Drawers/BoxCollider2DDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/CameraFollowDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/CircleCollider2DDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/NativeScriptDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/RigidBody2DDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/SpriteRendererDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/TransformDrawer.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/CircleCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Scripting/NativeScriptComponent.h"
#include "Engine/SceneSystem/Scene.h"

namespace mts
{
    InspectorPanel::InspectorPanel()
    {
        RegisterDefaultDrawers();
    }

    void InspectorPanel::Draw(Scene* scene, EditorContext& ctx)
    {
        ImGui::Begin("Inspector");
        ImGui::SetWindowFontScale(mFontScale);

        if (!scene)
        {
            ImGui::TextUnformatted("No active scene.");
            ImGui::End();
            return;
        }

        if (ctx.selected == UUID::InvalidID)
        {
            ImGui::TextUnformatted("No selection.");
            ImGui::End();
            return;
        }

        if (ctx.selected != mCachedId || (mCachedGO && mCachedGO->IsDead()))
        {
            mCachedId = ctx.selected;
            mCachedGO = scene->FindGameObjectByUUID(ctx.selected);
        }
        auto go = mCachedGO;
        if (!go)
        {
            ImGui::TextUnformatted("Selection invalid.");
            ImGui::End();
            return;
        }

        // Optional: always show basic info
        char nameBuf[256];
        snprintf(nameBuf, sizeof(nameBuf), "%s", go->GetName().c_str());
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
            go->SetName(nameBuf);

        // Layer
        const char* layerNames[] = { "Default", "Ground", "UI" };
        int layerIdx = static_cast<int>(go->GetLayer());
        if (ImGui::Combo("Layer", &layerIdx, layerNames, IM_ARRAYSIZE(layerNames)))
            go->SetLayer(static_cast<GameObject::LayerMasks>(layerIdx));

        // Tags
        std::vector<std::string> currentTags;
        for (const Tag& t : go->GetTags())
            currentTags.push_back(TagRegistry::GetName(t));

        ImGui::Text("Tags:");
        for (const auto& tagName : currentTags)
        {
            ImGui::SameLine();
            ImGui::TextUnformatted(tagName.c_str());
            ImGui::SameLine();
            ImGui::PushID(tagName.c_str());
            if (ImGui::SmallButton("x"))
                go->RemoveTag(tagName);
            ImGui::PopID();
        }
        static char sTagInput[64] = {};
        ImGui::InputText("##newtag", sTagInput, sizeof(sTagInput));
        ImGui::SameLine();
        if (ImGui::SmallButton("+ Tag") && sTagInput[0] != '\0')
        {
            go->AddTag(sTagInput);
            sTagInput[0] = '\0';
        }

        ImGui::Separator();

        // Modular drawers
        mRegistry.DrawAll(go);

        // Add Component
        ImGui::Separator();
        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponentPopup");

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            ImGui::BeginDisabled(go->GetComponent<Transform>() != nullptr);
            if (ImGui::MenuItem("Transform"))
                go->AddComponent<Transform>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<SpriteRenderer>() != nullptr);
            if (ImGui::MenuItem("SpriteRenderer"))
                go->AddComponent<SpriteRenderer>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<RigidBody2D>() != nullptr);
            if (ImGui::MenuItem("RigidBody2D"))
                go->AddComponent<RigidBody2D>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<BoxCollider2D>() != nullptr);
            if (ImGui::MenuItem("BoxCollider2D"))
                go->AddComponent<BoxCollider2D>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<CircleCollider2D>() != nullptr);
            if (ImGui::MenuItem("CircleCollider2D"))
                go->AddComponent<CircleCollider2D>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<NativeScriptComponent>() != nullptr);
            if (ImGui::MenuItem("NativeScriptComponent"))
                go->AddComponent<NativeScriptComponent>();
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void InspectorPanel::RegisterDefaultDrawers()
    {
        mRegistry.Add<TransformDrawer>();
        mRegistry.Add<SpriteRendererDrawer>();
        mRegistry.Add<RigidBody2DDrawer>();
        mRegistry.Add<BoxCollider2DDrawer>();
        mRegistry.Add<CircleCollider2DDrawer>();
        mRegistry.Add<NativeScriptDrawer>();
        mRegistry.Add<CameraFollowDrawer>();
    }
}  // namespace mts
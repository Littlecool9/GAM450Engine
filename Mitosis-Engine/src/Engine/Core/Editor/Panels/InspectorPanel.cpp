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

#include "Engine/Core/Editor/Inspector/Drawers/ActionListDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/ButtonDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/AnimationDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/AnimationSystemDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/AudioEmitterDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/AudioListenerDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/BoxCollider2DDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/CameraDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/CameraFollowDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/CircleCollider2DDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/NativeScriptDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/ParticleEmitterDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/RigidBody2DDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/SpriteRendererDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/TransformDrawer.h"
#include "Engine/Core/Editor/Inspector/Drawers/UITransformDrawer.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/Audio/AudioEmitter.h"
#include "Engine/EntitySystem/Component/Audio/AudioListener.h"
#include "Engine/EntitySystem/Component/Camera/Camera.h"
#include "Engine/EntitySystem/Component/Particles/ParticleEmitter.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/CircleCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/UITransform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/Scripting/NativeScriptComponent.h"
#include "Engine/Scripting/ScriptFactory.h"

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
            mCachedId = UUID::InvalidID;
            mCachedGO = nullptr;
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
        const char* layerNames[] = {"Default", "Ground", "UI"};
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
            ImGui::BeginDisabled(go->GetComponent<Transform>() != nullptr ||
                                 go->GetComponent<UITransform>() != nullptr);
            if (ImGui::MenuItem("Transform"))
                go->AddComponent<Transform>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<UITransform>() != nullptr ||
                                 go->GetComponent<Transform>() != nullptr);
            if (ImGui::MenuItem("UITransform"))
                go->AddComponent<UITransform>();
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

            ImGui::BeginDisabled(go->GetComponent<CameraComponent>() != nullptr);
            if (ImGui::MenuItem("Camera"))
                go->AddComponent<CameraComponent>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<AudioListener>() != nullptr);
            if (ImGui::MenuItem("AudioListener"))
                go->AddComponent<AudioListener>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<AudioEmitter>() != nullptr);
            if (ImGui::MenuItem("AudioEmitter"))
                go->AddComponent<AudioEmitter>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<Animation>() != nullptr);
            if (ImGui::MenuItem("Animation"))
                go->AddComponent<Animation>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<AnimationSystem>() != nullptr);
            if (ImGui::MenuItem("AnimationSystem"))
                go->AddComponent<AnimationSystem>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<ParticleEmitter>() != nullptr);
            if (ImGui::MenuItem("ParticleEmitter"))
                go->AddComponent<ParticleEmitter>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<ActionList>() != nullptr);
            if (ImGui::MenuItem("ActionList"))
                go->AddComponent<ActionList>();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(go->GetComponent<Button>() != nullptr);
            if (ImGui::MenuItem("Button"))
                go->AddComponent<Button>();
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Add Script"))
            ImGui::OpenPopup("AddScriptPopup");

        if (ImGui::BeginPopup("AddScriptPopup"))
        {
            auto& names = mts::ScriptFactory::GetRegisteredNames();
            if (names.empty())
            {
                ImGui::TextDisabled("(no scripts registered)");
            }
            else
            {
                for (const auto& name : names)
                {
                    if (ImGui::MenuItem(name.c_str()))
                    {
                        auto* nsc = go->AddComponent<NativeScriptComponent>();
                        nsc->SetClassName(name);
                        nsc->EnsureCreated();
                    }
                }
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void InspectorPanel::RegisterDefaultDrawers()
    {
        mRegistry.Add<TransformDrawer>();
        mRegistry.Add<UITransformDrawer>();
        mRegistry.Add<SpriteRendererDrawer>();
        mRegistry.Add<RigidBody2DDrawer>();
        mRegistry.Add<BoxCollider2DDrawer>();
        mRegistry.Add<CircleCollider2DDrawer>();
        mRegistry.Add<NativeScriptDrawer>();
        mRegistry.Add<CameraFollowDrawer>();
        mRegistry.Add<CameraDrawer>();
        mRegistry.Add<AudioListenerDrawer>();
        mRegistry.Add<AudioEmitterDrawer>();
        mRegistry.Add<AnimationDrawer>();
        mRegistry.Add<AnimationSystemDrawer>();
        mRegistry.Add<ParticleEmitterDrawer>();
        mRegistry.Add<ActionListDrawer>();
        mRegistry.Add<ButtonDrawer>();
    }
}  // namespace mts
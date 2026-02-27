/**
 * @file EditorLayer.cpp
 * @author sumin.park
 * @brief  layer for editing ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    EditorLayer::EditorLayer(EditorContext& ctx) : Layer("EditorLayer"), mCtx(ctx)
    {
    }

    void EditorLayer::OnImGuiRender(float dt)
    {
        if (!mCtx.showEditorUI)
            return;

        auto scene = SceneSystem::GetCurrentScene();

        // --- Fullscreen invisible host window for the DockSpace ---
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags hostFlags =
            ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_NoMove     |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("##DockHost", nullptr, hostFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockId = ImGui::GetID("MainDockspace");
        ImGui::DockSpace(dockId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

        // Build default layout once (only when no saved layout exists)
        if (ImGui::DockBuilderGetNode(dockId) == nullptr)
        {
            ImGui::DockBuilderRemoveNode(dockId);
            ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockId, viewport->Size);

            ImGuiID bottom, center, left, right;
            ImGui::DockBuilderSplitNode(dockId,  ImGuiDir_Down, 0.25f, &bottom, &center);
            ImGui::DockBuilderSplitNode(center,  ImGuiDir_Left, 0.20f, &left,   &right);

            ImGui::DockBuilderDockWindow("Hierarchy", left);
            ImGui::DockBuilderDockWindow("Inspector", right);
            ImGui::DockBuilderDockWindow("Assets",    bottom);
            ImGui::DockBuilderDockWindow("Log",       bottom);
            ImGui::DockBuilderDockWindow("Profiler",  bottom);
            ImGui::DockBuilderFinish(dockId);
        }

        ImGui::End(); // ##DockHost

        // Draw panels (dock automatically by name)
        mHierarchy.Draw(scene, mCtx);
        mInspector.Draw(scene, mCtx);
        mAssets.Draw(scene, mCtx);
        mLog.Draw(scene, mCtx);
        mProfiler.Draw(dt);
    }
}  // namespace mts
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
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/Renderer.h"
#include "Engine/Input/Input.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    EditorLayer::EditorLayer(EditorContext& ctx) : Layer("EditorLayer"), mCtx(ctx)
    {
    }

    void EditorLayer::OnUpdate(float dt)
    {
        if (mCtx.playState != PlayState::Edit)
            return;
        if (!mCtx.showEditorUI)
            return;

        bool ctrlHeld = Input::IsKeyDown(Key::LeftCtrl) || Input::IsKeyDown(Key::RightCtrl);
        if (ctrlHeld && Input::IsKeyPressed(Key::S))
        {
            auto* scene = SceneSystem::GetCurrentScene();
            if (scene && !scene->GetFilePath().empty())
                SceneSystem::SaveScene(scene, scene->GetFilePath());
        }

        UpdateEditorCamera(dt);
    }

    void EditorLayer::UpdateEditorCamera(float dt)
    {
        if (!mCtx.viewportHovered || ImGui::GetIO().WantCaptureKeyboard)
            return;

        const float speed = 5.0f / mCtx.editorCamera.zoom;
        glm::vec2& pos = mCtx.editorCamera.position;

        if (Input::IsKeyDown(Key::W))
            pos.y += speed * dt;
        if (Input::IsKeyDown(Key::S))
            pos.y -= speed * dt;
        if (Input::IsKeyDown(Key::A))
            pos.x -= speed * dt;
        if (Input::IsKeyDown(Key::D))
            pos.x += speed * dt;

        float scroll = Input::GetScrollDeltaY();
        if (scroll != 0.0f)
        {
            mCtx.editorCamera.zoom *= (1.0f + scroll * 0.1f);
            mCtx.editorCamera.zoom = glm::clamp(mCtx.editorCamera.zoom, 0.1f, 5.0f);
        }

        if (mCtx.viewportSize.x <= 0 || mCtx.viewportSize.y <= 0)
            return;

        constexpr float kPPU = PixelsPerUnit;
        float ppu = kPPU * mCtx.editorCamera.zoom;
        float logicW = mCtx.viewportSize.x / ppu;
        float logicH = mCtx.viewportSize.y / ppu;

        RenderCamera cam;
        cam.position = glm::vec3(pos, 0.0f);
        cam.view = glm::translate(glm::mat4(1.0f), -cam.position);
        cam.proj =
            glm::ortho(-logicW / 2.0f, logicW / 2.0f, -logicH / 2.0f, logicH / 2.0f, -1.0f, 1.0f);
        cam.viewProj = cam.proj * cam.view;
        cam.valid = true;
        Renderer::SetActiveCamera(cam);
    }

    void EditorLayer::HandleViewportInput(Scene* scene)
    {
        if (!mCtx.viewportHovered || ImGui::GetIO().WantCaptureMouse)
            return;

        ImVec2 mousePos = ImGui::GetMousePos();
        // ScreenToWorld expects viewport-local coords: x from left, y from BOTTOM (OpenGL)
        float vLocalX = mousePos.x - mCtx.viewportPos.x;
        float vLocalY = mCtx.viewportSize.y - (mousePos.y - mCtx.viewportPos.y);
        glm::vec2 worldPos = Renderer::ScreenToWorld({vLocalX, vLocalY});

        // Selection: left-click
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            UUID hit = UUID::InvalidID;
            float bestArea = FLT_MAX;
            for (auto* go : scene->GetGameObjects())
            {
                if (go->IsDead())
                    continue;
                auto* tr = go->GetComponent<mts::Transform>();
                if (!tr)
                    continue;
                glm::vec2 center = glm::vec2(tr->GetPosition());
                glm::vec2 halfExt = glm::vec2(tr->GetScale()) * 0.5f;
                if (worldPos.x >= center.x - halfExt.x && worldPos.x <= center.x + halfExt.x &&
                    worldPos.y >= center.y - halfExt.y && worldPos.y <= center.y + halfExt.y)
                {
                    float area = halfExt.x * halfExt.y * 4.0f;
                    if (area < bestArea)
                    {
                        bestArea = area;
                        hit = go->GetId();
                    }
                }
            }
            mCtx.selected = hit;
        }
    }

    void EditorLayer::DrawGizmo(Scene* scene)
    {
        if (mCtx.selected == UUID::InvalidID)
            return;
        auto* go = scene->FindGameObjectByUUID(mCtx.selected);
        if (!go)
            return;
        auto* tr = go->GetComponent<mts::Transform>();
        if (!tr)
            return;

        // WorldToScreen returns viewport-local pixels (x from left, y from top).
        // Add viewport origin to get absolute ImGui screen coordinates.
        glm::vec2 vpLocal = Renderer::WorldToScreen(tr->GetPosition());
        constexpr float arrowLen = 60.0f;
        ImVec2 origin = {vpLocal.x + mCtx.viewportPos.x, vpLocal.y + mCtx.viewportPos.y};
        ImVec2 xTip = {origin.x + arrowLen, origin.y};
        ImVec2 yTip = {origin.x, origin.y - arrowLen};

        auto* dl = ImGui::GetForegroundDrawList();
        dl->AddLine(origin, xTip, IM_COL32(225, 0, 0, 255), 2.5f);
        dl->AddLine(origin, yTip, IM_COL32(0, 225, 0, 255), 2.5f);
        dl->AddCircleFilled(origin, 5.0f, IM_COL32(255, 255, 255, 220));
    }

    void EditorLayer::OnImGuiRender(float dt)
    {
        if (!mCtx.showEditorUI)
        {
            Renderer::ClearGameViewport();
            return;
        }

        auto scene = SceneSystem::GetCurrentScene();

        // --- Fullscreen invisible host window for the DockSpace ---
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking |
                                     ImGuiWindowFlags_MenuBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("##DockHost", nullptr, hostFlags);
        ImGui::PopStyleVar(3);

        if (ImGui::BeginMenuBar())
        {
            const bool isPlaying = (mCtx.playState == PlayState::Play);

            if (isPlaying)
                ImGui::BeginDisabled();
            if (ImGui::Button("  Play  "))
                mCtx.requestPlay = true;
            if (isPlaying)
                ImGui::EndDisabled();

            ImGui::SameLine();

            if (!isPlaying)
                ImGui::BeginDisabled();
            if (ImGui::Button("  Stop  "))
                mCtx.requestStop = true;
            if (!isPlaying)
                ImGui::EndDisabled();

            ImGui::SameLine();

            if (isPlaying)
                ImGui::BeginDisabled();
            if (ImGui::Button("  Save  "))
            {
                if (!scene->GetFilePath().empty())
                    SceneSystem::SaveScene(scene, scene->GetFilePath());
            }
            ImGui::SameLine();
            if (ImGui::Button(" Reload "))
            {
                if (!scene->GetFilePath().empty())
                {
                    SceneSystem::OpenScene(scene->GetFilePath());
                }
            }
            if (isPlaying)
                ImGui::EndDisabled();

            // Aspect ratio selector
            ImGui::SameLine();
            ImGui::Separator();
            ImGui::SameLine();
            ImGui::TextUnformatted("Aspect:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90.0f);

            const char* aspectLabels[] = {"Free", "16:9", "4:3", "21:9", "Custom"};
            int currentAspect = static_cast<int>(Renderer::GetAspectMode());
            if (ImGui::Combo("##aspect", &currentAspect, aspectLabels, IM_ARRAYSIZE(aspectLabels)))
                Renderer::SetAspectMode(static_cast<AspectMode>(currentAspect));

            if (Renderer::GetAspectMode() == AspectMode::Custom)
            {
                float customX = Renderer::GetCustomAspectX();
                float customY = Renderer::GetCustomAspectY();
                ImGui::SameLine();
                ImGui::SetNextItemWidth(50.0f);
                bool changed = ImGui::InputFloat("##customX", &customX, 0.0f, 0.0f, "%.1f");
                ImGui::SameLine();
                ImGui::TextUnformatted(":");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(50.0f);
                changed |= ImGui::InputFloat("##customY", &customY, 0.0f, 0.0f, "%.1f");
                if (changed && customX > 0.0f && customY > 0.0f)
                    Renderer::SetAspectMode(AspectMode::Custom, customX, customY);
            }

            ImGui::EndMenuBar();
        }

        ImGuiID dockId = ImGui::GetID("MainDockspace");
        ImGui::DockSpace(dockId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

        // Build default layout once (only when no saved layout exists)
        if (ImGui::DockBuilderGetNode(dockId) == nullptr)
        {
            ImGui::DockBuilderRemoveNode(dockId);
            ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockId, viewport->Size);

            ImGuiID bottom, center, left, right;
            ImGui::DockBuilderSplitNode(dockId, ImGuiDir_Down, 0.25f, &bottom, &center);
            ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.20f, &left, &right);

            ImGui::DockBuilderDockWindow("Hierarchy", left);
            ImGui::DockBuilderDockWindow("Inspector", right);
            ImGui::DockBuilderDockWindow("Assets", bottom);
            ImGui::DockBuilderDockWindow("Log", bottom);
            ImGui::DockBuilderDockWindow("Profiler", bottom);
            ImGui::DockBuilderDockWindow("Scenes", bottom);
            ImGui::DockBuilderFinish(dockId);
        }

        // Push game viewport to match the dockspace central node
        auto* centralNode = ImGui::DockBuilderGetCentralNode(dockId);
        if (centralNode && centralNode->Size.x > 0 && centralNode->Size.y > 0)
        {
            ImGuiViewport* vp = ImGui::GetMainViewport();
            int nodeX = static_cast<int>(centralNode->Pos.x - vp->Pos.x);
            int nodeW = static_cast<int>(centralNode->Size.x);
            int nodeH = static_cast<int>(centralNode->Size.y);
            // ImGui Y is top-down; OpenGL Y is bottom-up
            int nodeY = static_cast<int>(vp->Size.y - (centralNode->Pos.y - vp->Pos.y) - nodeH);

            // Renderer applies the current aspect constraint and stores the result
            Renderer::SetAvailableViewport(nodeX, nodeY, nodeW, nodeH);

            // Read back the constrained viewport to keep viewportPos/viewportSize in ImGui space
            // (used by mouse picking and gizmo drawing)
            Viewport gv = Renderer::GetGameViewport();
            ImVec2 actualPos = {vp->Pos.x + gv.x, vp->Pos.y + vp->Size.y - gv.y - gv.h};
            mCtx.viewportPos = actualPos;
            mCtx.viewportSize = {static_cast<float>(gv.w), static_cast<float>(gv.h)};
            mCtx.viewportHovered =
                ImGui::IsMouseHoveringRect(actualPos,
                                           ImVec2(actualPos.x + gv.w, actualPos.y + gv.h));
        }
        else
        {
            mCtx.viewportHovered = false;
            Renderer::ClearGameViewport();
        }

        ImGui::End();  // ##DockHost

        // Draw panels (dock automatically by name)
        mHierarchy.Draw(scene, mCtx);
        mInspector.Draw(scene, mCtx);
        mAssets.Draw(scene, mCtx);
        mLog.Draw(scene, mCtx);
        mProfiler.Draw(dt);
        mScenes.Draw(mCtx);

        // Viewport interaction and gizmo (Edit mode only)
        if (mCtx.playState == PlayState::Edit)
        {
            HandleViewportInput(scene);
            DrawGizmo(scene);
        }
    }

    void EditorLayer::OnEnterPlayMode()
    {
        mCtx.selected = UUID::InvalidID;
        mCtx.playState = PlayState::Play;
    }

    void EditorLayer::OnExitPlayMode()
    {
        mCtx.selected = UUID::InvalidID;
        mCtx.playState = PlayState::Edit;
    }
}  // namespace mts
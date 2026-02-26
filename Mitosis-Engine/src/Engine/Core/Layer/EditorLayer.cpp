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
        mHierarchy.Draw(scene, mCtx);
        mInspector.Draw(scene, mCtx);
    }
}  // namespace mts
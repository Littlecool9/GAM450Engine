/**
 * @file EditorLayer.h
 * @author sumin.park
 * @brief  layer for editing ui
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Layer.h"
#include "Engine/Core/Editor/EditorContext.h"
#include "Engine/Core/Editor/Panels/HierarchyPanel.h"
#include "Engine/Core/Editor/Panels/InspectorPanel.h"
#include "Engine/Core/Editor/Panels/AssetPanel.h"
#include "Engine/Core/Editor/Panels/LogPanel.h"
#include "Engine/Core/Editor/Panels/ProfilerPanel.h"
#include "Engine/Core/Editor/Panels/ScenesPanel.h"

namespace mts
{
    class Scene;

    class EditorLayer : public Layer
    {
       public:
        explicit EditorLayer(EditorContext& ctx);

        void OnUpdate(float dt) override;
        void OnImGuiRender(float dt) override;

        void OnEnterPlayMode() override;
        void OnExitPlayMode() override;

       private:
        void UpdateEditorCamera(float dt);
        void HandleViewportInput(Scene* scene);
        void DrawGizmo(Scene* scene);

       private:
        EditorContext& mCtx;
        InspectorPanel mInspector;
        HierarchyPanel mHierarchy;
        AssetPanel mAssets;
        LogPanel mLog;
        ProfilerPanel mProfiler;
        ScenesPanel mScenes;
    };

}  // namespace mts
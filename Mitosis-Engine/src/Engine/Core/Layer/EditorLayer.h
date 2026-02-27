/**
 * @file EditorLayer.h
 * @author sumin.park
 * @brief  layer for editing ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
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

namespace mts
{
    class EditorLayer : public Layer
    {
       public:
        explicit EditorLayer(EditorContext& ctx);

        void OnImGuiRender(float dt) override;

       private:

       private:
        EditorContext& mCtx;
        InspectorPanel mInspector;
        HierarchyPanel mHierarchy;
        AssetPanel     mAssets;
        LogPanel       mLog;
        ProfilerPanel  mProfiler;
    };

}  // namespace mts
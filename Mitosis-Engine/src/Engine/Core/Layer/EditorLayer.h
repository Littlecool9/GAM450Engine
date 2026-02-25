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
#include "Engine/UUID/UUID.h"

namespace mts
{
    struct EditorContext
    {
        UUID selected = UUID::InvalidID;
        bool showEditorUI = true;
    };
    class EditorLayer : public Layer
    {
       public:
        explicit EditorLayer(EditorContext ctx);

        void OnImGuiRender(float dt) override;

       private:
        void DrawHierarchy();
        void DrawInspector();
        void DrawDockspace();

       private:
        EditorContext mCtx;
    };

}  // namespace mts
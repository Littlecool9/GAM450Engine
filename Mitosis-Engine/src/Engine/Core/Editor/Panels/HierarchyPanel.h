/**
 * @file HierarchyPanel.h
 * @author sumin.park
 * @brief hierarchy ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/EditorContext.h"

namespace mts
{
    class Scene;

    class HierarchyPanel
    {
       public:
        HierarchyPanel();
        void Draw(Scene* scene, EditorContext& ctx);

       private:
        float mFontScale = 1.3f;
    };

}  // namespace mts
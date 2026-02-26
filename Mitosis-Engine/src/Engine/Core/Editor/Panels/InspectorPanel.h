/**
 * @file InspectorPanel.h
 * @author sumin.park
 * @brief  inspector ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/EditorContext.h"
#include "Engine/Core/Editor/Inspector/InspectorRegistry.h"

namespace mts
{
    class Scene;

    class InspectorPanel
    {
       public:
        InspectorPanel();

        void Draw(Scene* scene, EditorContext& ctx);

       private:
        void RegisterDefaultDrawers();

       private:
        InspectorRegistry mRegistry;
    };

}  // namespace mts
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
    class GameObject;

    class InspectorPanel
    {
       public:
        InspectorPanel();

        void Draw(Scene* scene, EditorContext& ctx);

       private:
        void RegisterDefaultDrawers();

       private:
        InspectorRegistry mRegistry;
        UUID         mCachedId = UUID::InvalidID;
        GameObject*  mCachedGO = nullptr;
        Scene*       mCachedScene = nullptr;
        float        mFontScale = 1.3f;
    };

}  // namespace mts
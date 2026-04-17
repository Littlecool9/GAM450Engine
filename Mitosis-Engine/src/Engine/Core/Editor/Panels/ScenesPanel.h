/**
 * @file ScenesPanel.h
 * @author sumin.park
 * @brief  panel for managing the runtime scene registry
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Editor/EditorContext.h"

namespace mts
{
    class ScenesPanel
    {
       public:
        void Draw(EditorContext& ctx);

       private:
        std::string mRegistryPath = "Data/Properties/SceneRegistry.json";
    };

}  // namespace mts

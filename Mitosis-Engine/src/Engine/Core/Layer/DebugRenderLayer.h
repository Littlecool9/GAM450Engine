/**
 * @file DebugRenderLayer.h
 * @author sumin.park
 * @brief  layer for debug rendering
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    // must be pushed after scene layer
    class DebugRenderLayer : public Layer
    {
       public:
        explicit DebugRenderLayer();
        void OnAttach() override;
        void OnDetach() override;
        void OnDebugRender() override;

       private:
    };

}  // namespace mts
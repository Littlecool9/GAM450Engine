/**
 * @file MemoryManageLayer.h
 * @author sumin.park
 * @brief  layer for memory management
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    class MemoryManageLayer : public Layer
    {
       public:
        explicit MemoryManageLayer();

        // memory manager's initialization will be done outside of application
        // not adding init and shut down is intentional

        void OnBeginFrame() override;

        void OnImGuiRender(float dt) override;

       private:
    };

}  // namespace mts
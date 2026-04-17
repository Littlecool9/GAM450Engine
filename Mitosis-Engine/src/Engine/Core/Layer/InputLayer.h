/**
 * @file InputLayer.h
 * @author sumin.park
 * @brief  layer for input system
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    class InputLayer : public Layer
    {
       public:
        explicit InputLayer();

        void OnAttach() override;
        void OnDetach() override;

        void OnEvent(Event& e) override;

        void OnBeginFrame() override;

       private:
    };

}  // namespace mts
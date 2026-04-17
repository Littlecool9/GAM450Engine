/**
 * @file ResourceLayer.h
 * @author sumin.park
 * @brief  layer for resource manager
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    class ResourceLayer : public Layer
    {
       public:
        explicit ResourceLayer();
        void OnAttach() override;
        void OnDetach() override;

       private:
    };

}  // namespace mts
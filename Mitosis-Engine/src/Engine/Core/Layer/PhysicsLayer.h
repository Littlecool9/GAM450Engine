/**
 * @file PhysicsLayer.h
 * @author sumin.park
 * @brief  Layer for physics system
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    class PhysicsLayer : public Layer
    {
       public:
        explicit PhysicsLayer();
        void OnFixedUpdate(float fixedDt) override;
    };

}  // namespace mts
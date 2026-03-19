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
#include "Engine/Core/Editor/EditorContext.h"

namespace mts
{
    class PhysicsLayer : public Layer
    {
       public:
        explicit PhysicsLayer(EditorContext& ctx);
        void OnFixedUpdate(float fixedDt) override;

        void OnExitPlayMode() override;

       private:
        EditorContext& mCtx;
    };

}  // namespace mts
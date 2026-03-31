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
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class PhysicsLayer : public Layer
    {
       public:
        explicit PhysicsLayer(EditorContext& ctx);
        void OnAttach() override;
        void OnDetach() override;
        void OnFixedUpdate(float fixedDt) override;
        void OnExitPlayMode() override;

       private:
        EditorContext& mCtx;
        Delegate<>::Id mSceneChangeId = 0;
    };

}  // namespace mts
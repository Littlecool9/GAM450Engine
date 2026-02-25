/**
 * @file SceneLayer.h
 * @author sumin.park
 * @brief  Layer for scene system
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    class Scene;

    class SceneLayer : public Layer
    {
       public:
        explicit SceneLayer(Scene* initialScene);
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float dt) override;
        void OnLateUpdate(float dt) override;

        void OnRender() override;
        void OnDebugRender() override;

        void OnEndFrame() override;

       private:
        Scene* mInitialScene = nullptr;
    };

}  // namespace mts
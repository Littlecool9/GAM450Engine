/**
 * @file RenderingLayer.h
 * @author sumin.park
 * @brief  Layer for rendering system
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

namespace mts
{
    class WindowResizeEvent;
    class Window;
    class RenderingLayer : public Layer
    {
       public:
        explicit RenderingLayer(Window* window);
        void OnRender() override;

        void OnEvent(Event& e) override;

       private:
        bool OnWindowResize(WindowResizeEvent& e);
        Window* mWindow;
    };

}  // namespace mts
/**
 * @file ImGuiLayer.h
 * @author sumin.park
 * @brief  layer for ImGui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Layer.h"

struct GLFWwindow;

namespace mts
{
    class Window;

    class ImGuiLayer : public Layer
    {
       public:
        explicit ImGuiLayer(Window* window);

        void OnAttach() override;
        void OnDetach() override;

        void Begin();
        void End();

       private:
        Window* mWindow = nullptr;
        const char* mGlslVersion = "#version 460";
    };
}  // namespace mts
/**
 * @file Application.h
 * @author sumin.park
 * @brief The application class that the app will inherit.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#pragma once
#include "../Windows/Window.h"
#include "LayerStack.h"
#include <memory>

namespace mts::physics
{
    class PhysicsSystem2D;
}

namespace mts
{
    class Scene;
    class Application
    {
       public:
        Application() = default;
        virtual ~Application() = default;

        void Init();
        void Run();
        void Shutdown();

        void OnEvent(Event& e);
        void PushLayer(std::unique_ptr<Layer> layer) { mLayerStack.PushLayer(std::move(layer)); }
        void PushOverlay(std::unique_ptr<Layer> layer)
        {
            mLayerStack.PushOverlay(std::move(layer));
        }

       protected:
        virtual Scene* GetInitialScene() = 0;

       private:
        bool OnWindowClose(class WindowCloseEvent& e);

       private:
        std::unique_ptr<Window> mWindow;
        LayerStack mLayerStack;

        bool mIsRunning = false;
        bool mInDebugView = true;
    };

    Application* CreateApplication();
}  // namespace mts
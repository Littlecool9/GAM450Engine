/**
 * @file Application.h
 * @author sumin.park
 * @brief The application class that the app will inherit.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#pragma once
#include "Engine/Windows/Window.h"
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

       protected:
        virtual Scene* GetInitialScene() = 0;

       private:
        bool OnWindowClose(class WindowCloseEvent& e);
        bool OnWindowResize(class WindowResizeEvent& e);

       private:
        std::unique_ptr<Window> mWindow;
        bool mIsRunning = false;
    };

    Application* CreateApplication();
}  // namespace mts
/**
 * @file Application.h
 * @author sumin.park
 * @brief The application class that the app will inherit.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#pragma once
#include "Windows/Window.h"
#include "Audio/AudioManager.h"
#include "Conductor/ConductorSystem.h"
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
        void Run() const;
        void Shutdown();

        void OnEvent(Event& e);

       protected:
        virtual Scene* GetInitialScene() = 0;

       private:
        bool OnWindowClose(class WindowCloseEvent& e);
        static bool OnWindowResize(const class WindowResizeEvent& e);

       private:
        std::unique_ptr<Window> mWindow;
        std::unique_ptr<AudioManager> mAudio;
        std::unique_ptr<ConductorSystem> mConductor;
        bool mIsRunning = false;
        bool mInDebugView = false;
    };

    Application* CreateApplication();
}  // namespace mts
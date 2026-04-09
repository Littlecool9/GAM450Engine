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
#include "SplashScreen.h"
#include <memory>
#include <vector>

#include "Editor/EditorContext.h"
#include "Engine/Delegate/Delegate.h"

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
        virtual std::string& GetInitialScene() = 0;
        virtual std::string& GetSceneRegistry() = 0;
        virtual void RegisterScripts() {};
        virtual std::vector<SplashScreenConfig> GetSplashScreens() { return {}; }
        virtual void OnUpdate(float dt) {};

       private:
        void CloseWindow();
        void HandlePlayStateTransitions();
        void EnterPlayMode();
        void StopPlayMode();

       private:
        std::unique_ptr<Window> mWindow;
        LayerStack mLayerStack;

        EditorContext mCtx;

        bool mIsRunning = false;
        bool mInDebugView = true;
    };

    Application* CreateApplication();
}  // namespace mts
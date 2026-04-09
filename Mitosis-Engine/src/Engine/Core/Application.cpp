/**
 * @file Application.cpp
 * @author sumin.park
 * @brief The application class that the app will inherit.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Application.h"
#include "Engine/Windows/WinGLFWWindow.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Time/FPSCounter.h"
#include "Engine/Time/Time.h"
#include "Engine/Renderer.h"
#include "Engine/Helpers/Random.h"
#include "Layer/AudioLayer.h"
#include "Layer/DebugRenderLayer.h"
#include "Layer/EditorLayer.h"
#include "Layer/ImGuiLayer.h"
#include "Layer/InputLayer.h"
#include "Layer/MemoryManageLayer.h"
#include "Layer/PhysicsLayer.h"
#include "Layer/RenderingLayer.h"
#include "Layer/ResourceLayer.h"
#include "Layer/SceneLayer.h"
#include "Engine/AppSystem/AppSystem.h"
#include "Engine/Events/AppEvent.h"

namespace mts
{
    void Application::Init()
    {
        mWindow = Window::Create();
        mWindow->SetEventCallback([this](Event& e) { OnEvent(e); });

        Time::Init();
        Time::SetFixedDt(1.0 / 60.0);
        Time::SetTargetFps(0);

        Random::Init();

        mIsRunning = true;

        AppSystem::Get().SetEventCallback([this](Event& e) { OnEvent(e); });

        mCtx.showEditorUI = true;
        mCtx.selected = UUID::InvalidID;

        RegisterScripts();

        // push in order of calling
        mLayerStack.PushLayer(std::make_unique<MemoryManageLayer>());
        mLayerStack.PushLayer(std::make_unique<InputLayer>());
        mLayerStack.PushLayer(std::make_unique<ResourceLayer>());
        mLayerStack.PushLayer(std::make_unique<RenderingLayer>(mWindow.get()));
        mLayerStack.PushLayer(std::make_unique<AudioLayer>());
        mLayerStack.PushLayer(
            std::make_unique<SceneLayer>(GetInitialScene(), GetSceneRegistry(), mCtx));
        mLayerStack.PushLayer(std::make_unique<PhysicsLayer>(mCtx));
        mLayerStack.PushLayer(std::make_unique<DebugRenderLayer>());
        mLayerStack.PushOverlay(std::make_unique<ImGuiLayer>(mWindow.get()));
        mLayerStack.PushOverlay(std::make_unique<EditorLayer>(mCtx));
    }

    void Application::Run()
    {
        if (!mInDebugView)
        {
            SplashScreen::Run(GetSplashScreens(), *mWindow, mIsRunning);
            mCtx.requestPlay = true;
        }

        FPSCounter fps;
        ImGuiLayer* imgui = static_cast<ImGuiLayer*>(mLayerStack.FindLayerByName("ImGuiLayer"));

        MTS_ASSERT(imgui);

        while (mIsRunning)
        {
            HandlePlayStateTransitions();

            Time::BeginFrame();

            // MUST happen BEFORE PollEvents (so callbacks set pressed/released for this frame)
            for (auto& layer : mLayerStack)
                if (layer->IsEnabled())
                    layer->OnBeginFrame();

            mWindow->PollEvents();
            mWindow->ClearScreen();

            const float dt = static_cast<float>(Time::FrameDt());

            for (auto& layer : mLayerStack)
            {
                if (layer->IsEnabled())
                    layer->OnUpdate(dt);
            }

            if (Input::IsKeyPressed(Key::Grave))
            {
                mInDebugView = !mInDebugView;
                if (!mInDebugView)
                    Renderer::ClearGameViewport();
            }
            if (Input::IsKeyPressed(Key::F11))
                AppSystem::Get().ToggleFullscreen();

            this->OnUpdate(dt);

            for (auto& layer : mLayerStack)
            {
                if (layer->IsEnabled())
                    layer->OnLateUpdate(dt);
            }

            Time::StepFixed(
                [&](double fixedDt)
                {
                    float fixed = static_cast<float>(fixedDt);
                    for (auto& layer : mLayerStack)
                    {
                        if (layer->IsEnabled())
                            layer->OnFixedUpdate(fixed);
                    }
                });

            // OnRender is a stage for all layers to submit render commands
            Renderer::BeginFrame();
            for (auto& layer : mLayerStack)
            {
                if (layer->IsEnabled())
                    layer->OnRender();
            }

            // Split rendering: scene → post-process → UI
            Renderer::BeginSceneRender();  // Bind FBO if post-processing active
            Renderer::FlushScene();        // Opaque + transparent

            if (mInDebugView)
            {
                for (auto& layer : mLayerStack)
                    if (layer->IsEnabled())
                        layer->OnDebugRender();
            }

            Renderer::EndSceneRender();  // Execute post-processing, blit to screen
            Renderer::FlushUI();         // UI on top, always to default FB
            Renderer::EndFrame();

            if (mInDebugView)
            {
                fps.Update(dt);

                imgui->Begin();

                for (auto& layer : mLayerStack)
                    if (layer->IsEnabled())
                        layer->OnImGuiRender(dt);

                imgui->End();
            }

            mWindow->SwapBuffer();

            for (auto& layer : mLayerStack)
                if (layer->IsEnabled())
                    layer->OnEndFrame();

            Time::EndFrame();
        }
    }

    void Application::Shutdown()
    {
        mIsRunning = false;

        // Ensure play mode is exited before layers detach, so scene-owned systems
        // (e.g. RhythmSystem) can unsubscribe from conductor delegates while they're still alive.
        if (mCtx.playState == PlayState::Play)
            StopPlayMode();

        // Important to shutdown the opposite way from initializing

        // will shutdown in reverse order
        mLayerStack.Clear();

        mWindow.reset();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowCloseEvent>(
            [this](WindowCloseEvent& ev)
            {
                CloseWindow();
                return true;
            });

        dispatcher.Dispatch<AppQuitRequestEvent>(
            [this](AppQuitRequestEvent& ev)
            {
                CloseWindow();
                return true;
            });

        dispatcher.Dispatch<AppToggleFullscreenRequestEvent>(
            [this](AppToggleFullscreenRequestEvent& ev)
            {
                mWindow->SetFullscreen(!mWindow->IsFullscreen());
                return true;
            });

        // Dispatch to layers in reverse order (overlays first)
        for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
        {
            if (!(*it)->IsEnabled())
                continue;
            (*it)->OnEvent(e);

            if (e.Handled)  // if your Event has Handled flag
                break;
        }
    }

    void Application::CloseWindow()
    {
        mIsRunning = false;
    }

    void Application::HandlePlayStateTransitions()
    {
        if (mCtx.requestPlay && mCtx.playState == PlayState::Edit)
        {
            mCtx.requestPlay = false;
            EnterPlayMode();
        }
        else if (mCtx.requestStop && mCtx.playState == PlayState::Play)
        {
            mCtx.requestStop = false;
            StopPlayMode();
        }
    }

    void Application::EnterPlayMode()
    {
        for (auto& layer : mLayerStack)
        {
            if (layer->IsEnabled())
                layer->OnEnterPlayMode();
        }
    }

    void Application::StopPlayMode()
    {
        for (auto& layer : mLayerStack)
        {
            if (layer->IsEnabled())
                layer->OnExitPlayMode();
        }
    }

}  // namespace mts
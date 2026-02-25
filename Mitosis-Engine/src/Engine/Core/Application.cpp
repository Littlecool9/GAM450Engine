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

#include <glad/glad.h>

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

        EditorContext editCtx;
        editCtx.showEditorUI = true;

        // push in order of calling
        mLayerStack.PushLayer(std::make_unique<MemoryManageLayer>());
        mLayerStack.PushLayer(std::make_unique<InputLayer>());
        mLayerStack.PushLayer(std::make_unique<ResourceLayer>());
        mLayerStack.PushLayer(std::make_unique<RenderingLayer>(mWindow.get()));
        mLayerStack.PushLayer(std::make_unique<AudioLayer>());
        mLayerStack.PushLayer(std::make_unique<SceneLayer>(GetInitialScene()));
        mLayerStack.PushLayer(std::make_unique<PhysicsLayer>());
        mLayerStack.PushLayer(std::make_unique<DebugRenderLayer>());
        mLayerStack.PushOverlay(std::make_unique<ImGuiLayer>(mWindow.get()));
        mLayerStack.PushOverlay(std::make_unique<EditorLayer>(editCtx));
    }

    void Application::Run()
    {
        FPSCounter fps;
        ImGuiLayer* imgui = static_cast<ImGuiLayer*>(mLayerStack.FindLayerByName("ImGuiLayer"));

        MTS_ASSERT(imgui);

        while (mIsRunning)
        {
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

            for (auto& layer : mLayerStack)
            {
                if (layer->IsEnabled())
                    layer->OnRender();
            }

            if (mInDebugView)
            {
                for (auto& layer : mLayerStack)
                    if (layer->IsEnabled())
                        layer->OnDebugRender();

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

        // Important to shutdown the opposite way from initializing

        // will shutdown in reverse order
        mLayerStack.Clear();

        mWindow.reset();
    }

    void Application::OnEvent(Event& e)
    {
        // Dispatch to layers in reverse order (overlays first)
        for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it)
        {
            if (!(*it)->IsEnabled())
                continue;
            (*it)->OnEvent(e);

            if (e.Handled)  // if your Event has Handled flag
                break;
        }

        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& ev)
                                              { return OnWindowClose(ev); });

        if (Input::IsKeyPressed(Key::C))
            mInDebugView = !mInDebugView;
        if (Input::IsKeyPressed(Key::P))
            mts::SceneSystem::SaveScene(SceneSystem::GetCurrentScene(),
                                        "../TechDemo/Data/Resources/CurrentScene.json");
    }

    bool Application::OnWindowClose(class WindowCloseEvent& e)
    {
        mIsRunning = false;
        return true;
    }

}  // namespace mts
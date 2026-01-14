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
#include "Windows/WinGLFWWindow.h"
#include "glad/glad.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/WindowEvent.h"

#include "Input/Input.h"
#include "SceneSystem/SceneSystem.h"
#include "Time/FPSCounter.h"
#include "Time/Time.h"

namespace mts
{
    void Application::Init()
    {
        window = Window::Create();
        window->SetEventCallback([this](Event& e) { OnEvent(e); });

        // ---- GLAD sanity check ----
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);

        if (!vendor || !renderer || !version)
        {
            LOG_ERROR("OpenGL context or GLAD not initialized!");
        }
        else
        {
            LOG_INFO("OpenGL Vendor  : {}", reinterpret_cast<const char*>(vendor));
            LOG_INFO("OpenGL Renderer: {}", reinterpret_cast<const char*>(renderer));
            LOG_INFO("OpenGL Version : {}", reinterpret_cast<const char*>(version));
        }
        // ---------------------------

        glViewport(0, 0, window->GetWidth(), window->GetHeight());

        Time::Init();
        Time::SetFixedDt(1.0 / 60.0);
        Time::SetTargetFps(60);

        Input::Init();
        SceneSystem::Init(GetInitialScene());

        isRunning = true;
    }

    void Application::Run()
    {
        FPSCounter fps;

        while (isRunning)
        {
            // Clear "pressed/released this frame" BEFORE glfwPollEvents fires callbacks
            Time::BeginFrame();
            Input::BeginFrame();
            window->Update();

            const float dt = static_cast<float>(Time::FrameDt());

            SceneSystem::Update(dt);
            SceneSystem::LateUpdate(dt);

            SceneSystem::Render();

            if (fps.Update(dt))
            {
                LOG_INFO("FPS: {} ({:.2f} ms)", fps.FPSRounded(), fps.FrameMS());
            }

            Time::EndFrame();
        }
    }

    void Application::Shutdown()
    {
        isRunning = false;

        // Important to shutdown the opposite way from initializing
        SceneSystem::Shutdown();
        Input::Shutdown();
        window.reset();
    }

    void Application::OnEvent(Event& e)
    {
        Input::OnEvent(e);
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& ev)
                                              { return OnWindowClose(ev); });
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& ev)
                                               { return OnWindowResize(ev); });
    }

    bool Application::OnWindowClose(class WindowCloseEvent& e)
    {
        isRunning = false;
        return true;
    }

    bool Application::OnWindowResize(class WindowResizeEvent& e)
    {
        // TODO: Notify the render swapchain, etc.

        glViewport(0, 0, (GLsizei) e.GetWidth(), (GLsizei) e.GetHeight());

        LOG_INFO("Resize -> {}x{}", e.GetWidth(), e.GetHeight());
        return false;  // Allow others to react as well
    }
}  // namespace mts
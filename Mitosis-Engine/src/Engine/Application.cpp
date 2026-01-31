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
#include "Physics2D/PhysicsSystem2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

#if MTS_AUDIO_FMOD
#include <fmod_studio.hpp>
#include <fmod_errors.h>

static void TestFMODStudioInit()
{
    FMOD::Studio::System* studioSystem = nullptr;

    FMOD_RESULT result = FMOD::Studio::System::create(&studioSystem);

    if (result != FMOD_OK)
    {
        LOG_ERROR("[FMOD TEST] Failed to create Studio System : {}", FMOD_ErrorString(result));
        return;
    }

    result = studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);

    if (result != FMOD_OK)
    {
        LOG_ERROR("[FMOD TEST] Failed to initialize : {}", FMOD_ErrorString(result));
        studioSystem->release();
        return;
    }

    LOG_INFO("[FMOD TEST] FMOD Studio initialized successfully.");

    // Quick test one update tick
    studioSystem->update();

    LOG_INFO("[FMOD TEST] FMOD Studio update() successful.");

    studioSystem->release();

    LOG_INFO("[FMOD TEST] FMOD Studio shutdown cleanly.");
}

#endif

namespace mts
{
    void Application::Init()
    {
        mWindow = Window::Create();
        mWindow->SetEventCallback([this](Event& e) { OnEvent(e); });

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

        glViewport(0, 0, mWindow->GetWidth(), mWindow->GetHeight());

        Time::Init();
        Time::SetFixedDt(1.0 / 60.0);
        Time::SetTargetFps(60);

        Input::Init();

#if MTS_AUDIO_FMOD
        TestFMODStudioInit();
#endif

        SceneSystem::Init(GetInitialScene());

        mIsRunning = true;
    }

    void Application::Run()
    {
        FPSCounter fps;

        while (mIsRunning)
        {
            // Clear "pressed/released this frame" BEFORE glfwPollEvents fires callbacks
            Time::BeginFrame();
            Input::BeginFrame();
            mWindow->PollEvents();
            mWindow->ClearScreen();

            const float dt = static_cast<float>(Time::FrameDt());

            SceneSystem::Update(dt);
            SceneSystem::LateUpdate(dt);

            physics::PhysicsSystem2D::FixedUpdate(SceneSystem::GetCurrentScene(), dt);

            SceneSystem::Render();

            if (fps.Update(dt))
            {
                LOG_INFO("FPS: {} ({:.2f} ms)", fps.FPSRounded(), fps.FrameMS());
            }

            mWindow->SwapBuffer();
            Time::EndFrame();

        }
    }

    void Application::Shutdown()
    {
        mIsRunning = false;

        // Important to shutdown the opposite way from initializing
        SceneSystem::Shutdown();
        Input::Shutdown();
        mWindow.reset();
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
        mIsRunning = false;
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
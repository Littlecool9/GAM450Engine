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

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Debug/DebugDraw.h"
#include "Windows/WinGLFWWindow.h"
#include "glad/glad.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"
#include "imgui.h"
#include "Input/Input.h"
#include "SceneSystem/SceneSystem.h"
#include "Time/FPSCounter.h"
#include "Time/Time.h"
#include "Physics2D/PhysicsSystem2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Resource/ResourceManager.h"
#include "Audio/AudioManager.h"
#include "Renderer.h"
#include "Helpers/Random.h"

#ifdef MTS_CI
#define MTS_AUDIO_USE_FMOD 0
#else
#define MTS_AUDIO_USE_FMOD 1
#endif

#if MTS_AUDIO_USE_FMOD
#include "Audio/Backends/FMODBackend.h"
#else
#include "Audio/Backends/NullAudioBackend.h"
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
        Time::SetTargetFps(0);

        Random::Init();

        Input::Init();
        mAudio = std::make_unique<AudioManager>();

        AudioManager::SetManager(mAudio.get());

        AudioConfig audioConfig{};
        audioConfig.liveUpdate = true;  // Dev only
        audioConfig.maxVirtualVoices = 1024;
        audioConfig.maxRealVoices = 64;

        // Create backend
        std::unique_ptr<IAudioBackend> backend;

#if defined(MTS_AUDIO_FMOD) && !defined(MTS_CI)
        backend = std::make_unique<FMODBackend>();
#else
        backend = std::make_unique<NullAudioBackend>();
#endif

        if (!mAudio->Init(std::move(backend), audioConfig))
        {
            LOG_CRITICAL("AudioManager Init failed.");
        }
        else
        {
            LOG_INFO("AudioManager Init OK.");
        }
        mAudio->AddBankToGroup(BankGroup::Core, "Assets/Audio/Desktop/Master.bank");
        mAudio->AddBankToGroup(BankGroup::Core, "Assets/Audio/Desktop/Master.strings.bank");
        mAudio->AddBankToGroup(BankGroup::Music, "Assets/Audio/Desktop/Music.bank");
        mAudio->AddBankToGroup(BankGroup::Level, "Assets/Audio/Desktop/SFX.bank");
        mAudio->LoadBankGroup(BankGroup::Core);
        mAudio->LoadBankGroup(BankGroup::Music);
        mAudio->LoadBankGroup(BankGroup::Level);
        mIsRunning = true;

        ResourceManager::Init();
        SceneSystem::Init(GetInitialScene());
        debug::DebugDraw::Init();

        // TODO: Decide whether ImGUI Code stays here or seperate class
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(mWindow->NativeWindow());
        ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
        ImGui_ImplOpenGL3_Init("#version 450");  // or "#version 330"
        // END IMGUI CODE
    }

    void Application::Run() const
    {
        FPSCounter fps;

        while (mIsRunning)
        {
            // Clear "pressed/released this frame" BEFORE glfwPollEvents fires callbacks
            Time::BeginFrame();
            Input::BeginFrame();
            MemoryManager::BeginFrame();
            mWindow->PollEvents();
            mWindow->ClearScreen();

            const float dt = static_cast<float>(Time::FrameDt());

            SceneSystem::Update(dt);
            SceneSystem::LateUpdate(dt);
            mAudio->Update(dt);
            physics::PhysicsSystem2D::FixedUpdate(SceneSystem::GetCurrentScene(), dt);

            Renderer::DrawSprites(SceneSystem::GetCurrentScene());
            SceneSystem::Render();
            if (mInDebugView)
            {
                SceneSystem::DebugRender();
                auto camera = Renderer::GetActiveCamera();
                debug::DebugDraw::Draw(camera.viewProj);

                if (fps.Update(dt))
                {
                    LOG_INFO("FPS: {} ({:.2f} ms)", fps.FPSRounded(), fps.FrameMS());
                }

                // Begin ImGui frame TODO: Decide whether ImGUI Code stays here or seperate class
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                MemoryManager::MemorymaDrawProfilerWindow(dt);

                // End ImGui frame (still before swap)
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                // END IMGUI CODE
            }

            mWindow->SwapBuffer();

            SceneSystem::SceneCleanUp();

            Time::EndFrame();
        }
    }

    void Application::Shutdown()
    {
        mIsRunning = false;

        // Important to shutdown the opposite way from initializing
        debug::DebugDraw::Shutdown();
        SceneSystem::Shutdown();
        ResourceManager::Shutdown();
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
        if (Input::IsKeyPressed(Key::C))
            mInDebugView = !mInDebugView;
    }

    bool Application::OnWindowClose(class WindowCloseEvent& e)
    {
        mIsRunning = false;
        return true;
    }

    bool Application::OnWindowResize(const class WindowResizeEvent& e)
    {
        // TODO: Notify the render swap chain, etc.

        glViewport(0, 0, static_cast<GLsizei>(e.GetWidth()), static_cast<GLsizei>(e.GetHeight()));

        LOG_INFO("Resize -> {}x{}", e.GetWidth(), e.GetHeight());
        return false;  // Allow others to react as well
    }
}  // namespace mts
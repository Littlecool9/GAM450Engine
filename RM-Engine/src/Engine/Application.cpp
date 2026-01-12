/**
 * @file Application.cpp
 * @author sumin.park
 * @brief The application class that the app will inherit.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "Application.h"
#include "Windows/WinGLFWWindow.h"
#include "glad/glad.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/WindowEvent.h"

#include "Input/Input.h"

namespace {
    // ---- Simple triangle test (local to this .cpp) ----
    static GLuint s_TriangleVAO = 0;
    static GLuint s_TriangleVBO = 0;
    static GLuint s_TriangleProgram = 0;

    static void LogShaderError(GLuint shader, const char* label) {
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_TRUE)
            return;

        GLchar infoLog[1024]{};
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        LOG_ERROR("[Shader Compile Failed: {}]\n{}", label, infoLog);
    }

    static void LogProgramError(GLuint program) {
        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (success == GL_TRUE)
            return;

        GLchar infoLog[1024]{};
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        LOG_ERROR("[Program Link Failed]\n{}", infoLog);
    }

    static bool CreateTriangleTest() {
        // Minimal triangle in NDC
        const float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

        const char* vertexSrc = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			void main()
			{
				gl_Position = vec4(aPos, 1.0);
			}
		)";

        const char* fragmentSrc = R"(
			#version 330 core
			out vec4 FragColor;
			void main()
			{
				FragColor = vec4(1.0, 0.4, 0.2, 1.0);
			}
		)";

        // Compile shaders
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertexSrc, nullptr);
        glCompileShader(vs);
        LogShaderError(vs, "Triangle Vertex Shader");

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragmentSrc, nullptr);
        glCompileShader(fs);
        LogShaderError(fs, "Triangle Fragment Shader");

        // Link program
        s_TriangleProgram = glCreateProgram();
        glAttachShader(s_TriangleProgram, vs);
        glAttachShader(s_TriangleProgram, fs);
        glLinkProgram(s_TriangleProgram);
        LogProgramError(s_TriangleProgram);

        glDeleteShader(vs);
        glDeleteShader(fs);

        // Create VAO/VBO
        glGenVertexArrays(1, &s_TriangleVAO);
        glGenBuffers(1, &s_TriangleVBO);

        glBindVertexArray(s_TriangleVAO);

        glBindBuffer(GL_ARRAY_BUFFER, s_TriangleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Basic sanity: program/vao must exist
        if (s_TriangleProgram == 0 || s_TriangleVAO == 0) {
            LOG_ERROR("Triangle test creation failed (program or VAO is 0).");
            return false;
        }

        return true;
    }

    static void DestroyTriangleTest() {
        if (s_TriangleVAO) {
            glDeleteVertexArrays(1, &s_TriangleVAO);
            s_TriangleVAO = 0;
        }
        if (s_TriangleVBO) {
            glDeleteBuffers(1, &s_TriangleVBO);
            s_TriangleVBO = 0;
        }
        if (s_TriangleProgram) {
            glDeleteProgram(s_TriangleProgram);
            s_TriangleProgram = 0;
        }
    }
    // -----------------------------------------------
}  // namespace

namespace rm {
    void Application::Init() {
        window = Window::Create();
        window->SetEventCallback([this](Event& e) { OnEvent(e); });

        // ---- GLAD sanity check ----
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);

        if (!vendor || !renderer || !version) {
            LOG_ERROR("OpenGL context or GLAD not initialized!");
        } else {
            LOG_INFO("OpenGL Vendor  : {}", reinterpret_cast<const char*>(vendor));
            LOG_INFO("OpenGL Renderer: {}", reinterpret_cast<const char*>(renderer));
            LOG_INFO("OpenGL Version : {}", reinterpret_cast<const char*>(version));
        }
        // ---------------------------

        glViewport(0, 0, window->GetWidth(), window->GetHeight());

        if (!CreateTriangleTest()) {
            LOG_ERROR("Failed to create triangle test (GL pipeline not working).");
        }

        Input::Init();

        isRunning = true;
    }

    void Application::Run() {
        while (isRunning) {
            // Clear "pressed/released this frame" BEFORE glfwPollEvents fires callbacks
            Input::BeginFrame();
            window->Update();

            // --- Render test triangle ---
            glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (s_TriangleProgram && s_TriangleVAO) {
                glUseProgram(s_TriangleProgram);
                glBindVertexArray(s_TriangleVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
            }
            // ----------------------------

            if (Input::IsKeyPressed(Key::Space)) {
                LOG_INFO("SPACE PRESSED");
            }

            if (Input::IsKeyDown(Key::Space)) {
                LOG_INFO("SPACE HELD");
            }

            if (Input::IsKeyReleased(Key::Space)) {
                LOG_INFO("SPACE RELEASED");
            }
        }
    }

    void Application::Shutdown() {
        isRunning = false;

        DestroyTriangleTest();

        Input::Shutdown();
        window.reset();
    }

    void Application::OnEvent(Event& e) {
        Input::OnEvent(e);
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowCloseEvent>(
            [this](WindowCloseEvent& ev) { return OnWindowClose(ev); });
        dispatcher.Dispatch<WindowResizeEvent>(
            [this](WindowResizeEvent& ev) { return OnWindowResize(ev); });
    }

    bool Application::OnWindowClose(class WindowCloseEvent& e) {
        isRunning = false;
        return true;
    }

    bool Application::OnWindowResize(class WindowResizeEvent& e) {
        // TODO: Notify the render swapchain, etc.

        glViewport(0, 0, (GLsizei) e.GetWidth(), (GLsizei) e.GetHeight());

        LOG_INFO("Resize -> {}x{}", e.GetWidth(), e.GetHeight());
        return false;  // Allow others to react as well
    }
}  // namespace rm
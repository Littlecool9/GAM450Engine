/**
 * @file RenderingLayer.cpp
 * @author sumin.park
 * @brief  Layer for rendering system
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "RenderingLayer.h"

#include <glad/glad.h>

#include "Engine/Renderer.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Windows/Window.h"
#include "Engine/Events/WindowEvent.h"

namespace mts
{
    RenderingLayer::RenderingLayer(Window* window) : Layer("RenderingLayer"), mWindow(window)
    {
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
    }

    void RenderingLayer::OnRender()
    {
        Renderer::DrawSprites(SceneSystem::GetCurrentScene());
    }

    void RenderingLayer::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& ev)
                                               { return OnWindowResize(ev); });
    }

    bool RenderingLayer::OnWindowResize(WindowResizeEvent& e)
    {
        // TODO: Notify the render swap chain, etc.

        glViewport(0, 0, static_cast<GLsizei>(e.GetWidth()), static_cast<GLsizei>(e.GetHeight()));

        LOG_INFO("Resize -> {}x{}", e.GetWidth(), e.GetHeight());
        return false;  // Allow others to react as well
    }
}  // namespace mts
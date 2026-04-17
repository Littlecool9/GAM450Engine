/**
 * @file Window.cpp
 * @author sumin.park
 * @brief The interface representing the window.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "Window.h"

#include "WinGLFWwindow.h"

namespace mts
{
    std::unique_ptr<Window> Window::Create(const WindowProps& props)
    {
        return std::make_unique<WinGLFWwindow>(props);
    }
}  // namespace mts
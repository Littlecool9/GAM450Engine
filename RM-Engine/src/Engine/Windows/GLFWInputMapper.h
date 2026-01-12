/**
 * @file GLFWInputMapper.h
 * @author rahul
 * @brief // Maps GLFW-specific input codes to engine-defined input codes.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

#include <GLFW/glfw3.h>
#include "Engine/Input/InputCodes.h"

namespace rm
{
    struct GLFWInputMapper
    {
        static Key ToKey(int glfwKey)
        {
            // Letters
            if (glfwKey >= GLFW_KEY_A && glfwKey <= GLFW_KEY_Z)
                return static_cast<Key>(static_cast<int>(Key::A) + (glfwKey - GLFW_KEY_A));

            // Numbers
            if (glfwKey >= GLFW_KEY_0 && glfwKey <= GLFW_KEY_9)
                return static_cast<Key>(static_cast<int>(Key::D0) + (glfwKey - GLFW_KEY_0));

            switch (glfwKey)
            {
            case GLFW_KEY_SPACE:
                return Key::Space;
            case GLFW_KEY_ENTER:
                return Key::Enter;
            case GLFW_KEY_ESCAPE:
                return Key::Escape;
            case GLFW_KEY_TAB:
                return Key::Tab;
            case GLFW_KEY_BACKSPACE:
                return Key::Backspace;

            case GLFW_KEY_LEFT:
                return Key::Left;
            case GLFW_KEY_RIGHT:
                return Key::Right;
            case GLFW_KEY_UP:
                return Key::Up;
            case GLFW_KEY_DOWN:
                return Key::Down;

            case GLFW_KEY_LEFT_SHIFT:
                return Key::LeftShift;
            case GLFW_KEY_RIGHT_SHIFT:
                return Key::RightShift;
            case GLFW_KEY_LEFT_CONTROL:
                return Key::LeftCtrl;
            case GLFW_KEY_RIGHT_CONTROL:
                return Key::RightCtrl;
            case GLFW_KEY_LEFT_ALT:
                return Key::LeftAlt;
            case GLFW_KEY_RIGHT_ALT:
                return Key::RightAlt;

            default:
                return Key::Unknown;
            }
        }

        static MouseButton ToMouseButton(int glfwButton)
        {
            switch (glfwButton)
            {
            case GLFW_MOUSE_BUTTON_LEFT:
                return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return MouseButton::Middle;
            case GLFW_MOUSE_BUTTON_4:
                return MouseButton::Button4;
            case GLFW_MOUSE_BUTTON_5:
                return MouseButton::Button5;
            default:
                return MouseButton::Unknown;
            }
        }
    };
} // rm namespace
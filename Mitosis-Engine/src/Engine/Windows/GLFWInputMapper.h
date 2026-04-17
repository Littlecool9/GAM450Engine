/**
 * @file GLFWInputMapper.h
 * @author rahul
 * @brief // Maps GLFW-specific input codes to engine-defined input codes.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include <GLFW/glfw3.h>
#include "Engine/Input/InputCodes.h"

namespace mts
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

            // Function Keys
            if (glfwKey >= GLFW_KEY_F1 && glfwKey <= GLFW_KEY_F12)
                return static_cast<Key>(static_cast<int>(Key::F1) + (glfwKey - GLFW_KEY_F1));

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

                case GLFW_KEY_GRAVE_ACCENT:
                    return Key::Grave;

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

        // GamepadButton values are defined to match GLFW_GAMEPAD_BUTTON_* — cast directly.
        static GamepadButton ToGamepadButton(int glfwButton)
        {
            if (glfwButton >= 0 && glfwButton <= GLFW_GAMEPAD_BUTTON_LAST)
                return static_cast<GamepadButton>(glfwButton);
            return GamepadButton::Unknown;
        }

        // GamepadAxis values are defined to match GLFW_GAMEPAD_AXIS_* — cast directly.
        static GamepadAxis ToGamepadAxis(int glfwAxis)
        {
            if (glfwAxis >= 0 && glfwAxis <= GLFW_GAMEPAD_AXIS_LAST)
                return static_cast<GamepadAxis>(glfwAxis);
            return GamepadAxis::Unknown;
        }
    };

    // Compile-time verification that our enums stay in sync with GLFW constants.
    static_assert(static_cast<int>(GamepadButton::A)            == GLFW_GAMEPAD_BUTTON_A);
    static_assert(static_cast<int>(GamepadButton::B)            == GLFW_GAMEPAD_BUTTON_B);
    static_assert(static_cast<int>(GamepadButton::X)            == GLFW_GAMEPAD_BUTTON_X);
    static_assert(static_cast<int>(GamepadButton::Y)            == GLFW_GAMEPAD_BUTTON_Y);
    static_assert(static_cast<int>(GamepadButton::LeftBumper)   == GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
    static_assert(static_cast<int>(GamepadButton::RightBumper)  == GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);
    static_assert(static_cast<int>(GamepadButton::Back)         == GLFW_GAMEPAD_BUTTON_BACK);
    static_assert(static_cast<int>(GamepadButton::Start)        == GLFW_GAMEPAD_BUTTON_START);
    static_assert(static_cast<int>(GamepadButton::Guide)        == GLFW_GAMEPAD_BUTTON_GUIDE);
    static_assert(static_cast<int>(GamepadButton::LeftThumb)    == GLFW_GAMEPAD_BUTTON_LEFT_THUMB);
    static_assert(static_cast<int>(GamepadButton::RightThumb)   == GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);
    static_assert(static_cast<int>(GamepadButton::DPadUp)       == GLFW_GAMEPAD_BUTTON_DPAD_UP);
    static_assert(static_cast<int>(GamepadButton::DPadRight)    == GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
    static_assert(static_cast<int>(GamepadButton::DPadDown)     == GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
    static_assert(static_cast<int>(GamepadButton::DPadLeft)     == GLFW_GAMEPAD_BUTTON_DPAD_LEFT);

    static_assert(static_cast<int>(GamepadAxis::LeftX)          == GLFW_GAMEPAD_AXIS_LEFT_X);
    static_assert(static_cast<int>(GamepadAxis::LeftY)          == GLFW_GAMEPAD_AXIS_LEFT_Y);
    static_assert(static_cast<int>(GamepadAxis::RightX)         == GLFW_GAMEPAD_AXIS_RIGHT_X);
    static_assert(static_cast<int>(GamepadAxis::RightY)         == GLFW_GAMEPAD_AXIS_RIGHT_Y);
    static_assert(static_cast<int>(GamepadAxis::LeftTrigger)    == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER);
    static_assert(static_cast<int>(GamepadAxis::RightTrigger)   == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER);

}  // namespace mts
/**
 * @file InputCodes.h
 * @author rahul
 * @brief // Engine-defined, platform-agnostic key and mouse codes used by the input system.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    enum class Key : uint16_t
    {
        Unknown = 0,

        // Letters
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        // Numbers
        D0,
        D1,
        D2,
        D3,
        D4,
        D5,
        D6,
        D7,
        D8,
        D9,

        // Controls
        Space,
        Enter,
        Escape,
        Tab,
        Backspace,

        // Arrows
        Left,
        Right,
        Up,
        Down,

        // Modifiers
        LeftShift,
        RightShift,
        LeftCtrl,
        RightCtrl,
        LeftAlt,
        RightAlt,

        // Special
        Grave,  // backtick / tilde (`)

        // Function Keys
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12
    };

    enum class MouseButton : uint8_t
    {
        Left = 0,
        Right = 1,
        Middle = 2,
        Button4 = 3,
        Button5 = 4,
        Unknown = 255
    };

    // Values match GLFW_GAMEPAD_BUTTON_* constants — verified by static_assert in
    // GLFWInputMapper.h.
    enum class GamepadButton : uint8_t
    {
        A = 0,  // Cross on PlayStation
        B,      // Circle
        X,      // Square
        Y,      // Triangle

        LeftBumper,
        RightBumper,

        Back,
        Start,
        Guide,

        LeftThumb,
        RightThumb,

        DPadUp,
        DPadRight,
        DPadDown,
        DPadLeft,

        Count,
        Unknown = 255
    };

    // Values match GLFW_GAMEPAD_AXIS_* constants — verified by static_assert in
    // GLFWInputMapper.h.
    enum class GamepadAxis : uint8_t
    {
        LeftX = 0,
        LeftY,
        RightX,
        RightY,
        LeftTrigger,   // -1.0 (released) → 1.0 (fully pressed)
        RightTrigger,

        Count,
        Unknown = 255
    };

}  // namespace mts
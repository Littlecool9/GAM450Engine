/**
 * @file Input.h
 * @author rahul
 * @brief // Centralized, platform-independent input service providing frame-based input queries.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include <utility>
#include "InputCodes.h"
#include "Engine/Events/Event.h"

namespace mts
{
    class Input
    {
       public:
        // Call once on engine start (no platform knowledge)
        static void Init();
        static void Shutdown();

        // Call once per frame BEFORE platform pumps events
        static void BeginFrame();

        // --- Queries (Engine-facing) ---

        static bool IsKeyDown(Key key);
        static bool IsKeyPressed(Key key);
        static bool IsKeyReleased(Key key);

        static bool IsMouseDown(MouseButton button);
        static bool IsMousePressed(MouseButton button);
        static bool IsMouseReleased(MouseButton button);

        static float GetMouseX();
        static float GetMouseY();
        static std::pair<float, float> GetMousePosition();
        static float GetScrollDeltaY();

        // --- Gamepad queries (Engine-facing) ---

        static constexpr int MaxGamepads = 4;

        static bool IsGamepadConnected(int padId = 0);

        static bool IsGamepadButtonDown(GamepadButton button, int padId = 0);
        static bool IsGamepadButtonPressed(GamepadButton button, int padId = 0);
        static bool IsGamepadButtonReleased(GamepadButton button, int padId = 0);

        static float GetGamepadAxis(GamepadAxis axis, int padId = 0);

        // --- Event sinks (Platform-facing) ---

        static void OnKey(Key key, bool down);
        static void OnMouseButton(MouseButton button, bool down);
        static void OnMouseMove(float x, float y);
        static void OnScroll(float yOffset);
        static void OnEvent(Event& event);

        static void OnGamepadConnected(int padId);
        static void OnGamepadDisconnected(int padId);
        // buttons: GLFW_PRESS/RELEASE as bool, length buttonCount; axes: float, length axisCount
        static void OnGamepadPoll(int padId, const bool* buttons, int buttonCount,
                                  const float* axes, int axisCount);

        static void ClearAll();

       private:
        Input() = delete;

        struct GamepadPadState
        {
            static constexpr int MaxButtons = static_cast<int>(GamepadButton::Count);
            static constexpr int MaxAxes    = static_cast<int>(GamepadAxis::Count);

            bool connected = false;
            bool buttons[MaxButtons]{};
            bool prevButtons[MaxButtons]{};
            float axes[MaxAxes]{};
        };

        struct State
        {
            static constexpr int MaxKeys = 512;
            static constexpr int MaxMouseButtons = 16;

            bool keys[MaxKeys]{};
            bool prevKeys[MaxKeys]{};

            bool mouse[MaxMouseButtons]{};
            bool prevMouse[MaxMouseButtons]{};

            float mouseX = 0.0f;
            float mouseY = 0.0f;
            float scrollDeltaY = 0.0f;

            GamepadPadState gamepads[MaxGamepads]{};

            bool initialized = false;
        };

        static State& GetState();
        static int KeyIndex(Key key);
        static int MouseIndex(MouseButton button);
    };

}  // namespace mts
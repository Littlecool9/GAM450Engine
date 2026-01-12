/**
 * @file Input.h
 * @author rahul
 * @brief // Centralized, platform-independent input service providing frame-based input queries.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once

#include <utility>
#include "InputCodes.h"
#include "Engine/Events/Event.h"

namespace rm
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

        // --- Event sinks (Platform-facing) ---

        static void OnKey(Key key, bool down);
        static void OnMouseButton(MouseButton button, bool down);
        static void OnMouseMove(float x, float y);
        static void OnScroll(float yOffset);
        static void OnEvent(Event &event);

        static void ClearAll();

    private:
        Input() = delete;

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

            bool initialized = false;
        };

        static State &GetState();
        static int KeyIndex(Key key);
        static int MouseIndex(MouseButton button);
    };

} // rm namespace
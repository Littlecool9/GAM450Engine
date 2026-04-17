/**
 * @file Input.cpp
 * @author rahul
 * @brief // Centralized, platform-independent input service providing frame-based input queries.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "Input.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/InputEvent.h"
#include "Engine/Events/WindowEvent.h"

namespace mts
{
    Input::State& Input::GetState()
    {
        static State s{};
        return s;
    }

    int Input::KeyIndex(Key key)
    {
        auto v = static_cast<int>(key);
        if (v <= 0 || v >= State::MaxKeys)
            return -1;
        return v;
    }

    int Input::MouseIndex(MouseButton button)
    {
        int v = (button == MouseButton::Unknown) ? -1 : static_cast<int>(button);
        if (v < 0 || v >= State::MaxMouseButtons)
            return -1;
        return v;
    }

    void Input::Init()
    {
        auto& s = GetState();
        s = State{};
        s.initialized = true;
    }

    void Input::Shutdown()
    {
        auto& s = GetState();
        s = State{};
    }

    void Input::BeginFrame()
    {
        auto& s = GetState();
        MTS_ASSERT(s.initialized && "Input::Init must be called before BeginFrame.");

        std::memcpy(s.prevKeys, s.keys, sizeof(s.keys));
        std::memcpy(s.prevMouse, s.mouse, sizeof(s.mouse));
        s.scrollDeltaY = 0.0f;

        for (int i = 0; i < MaxGamepads; ++i)
            std::memcpy(s.gamepads[i].prevButtons,
                        s.gamepads[i].buttons,
                        sizeof(s.gamepads[i].buttons));
    }

    // -------- Queries --------

    bool Input::IsKeyDown(Key key)
    {
        const auto& s = GetState();
        int i = KeyIndex(key);
        return i >= 0 && s.keys[i];
    }

    bool Input::IsKeyPressed(Key key)
    {
        const auto& s = GetState();
        int i = KeyIndex(key);
        return i >= 0 && s.keys[i] && !s.prevKeys[i];
    }

    bool Input::IsKeyReleased(Key key)
    {
        const auto& s = GetState();
        int i = KeyIndex(key);
        return i >= 0 && !s.keys[i] && s.prevKeys[i];
    }

    bool Input::IsMouseDown(MouseButton button)
    {
        const auto& s = GetState();
        int i = MouseIndex(button);
        return i >= 0 && s.mouse[i];
    }

    bool Input::IsMousePressed(MouseButton button)
    {
        const auto& s = GetState();
        int i = MouseIndex(button);
        return i >= 0 && s.mouse[i] && !s.prevMouse[i];
    }

    bool Input::IsMouseReleased(MouseButton button)
    {
        const auto& s = GetState();
        int i = MouseIndex(button);
        return i >= 0 && !s.mouse[i] && s.prevMouse[i];
    }

    float Input::GetMouseX()
    {
        return GetState().mouseX;
    }
    float Input::GetMouseY()
    {
        return GetState().mouseY;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        const auto& s = GetState();
        return {s.mouseX, s.mouseY};
    }

    float Input::GetScrollDeltaY()
    {
        return GetState().scrollDeltaY;
    }

    // -------- Event sinks (called by platform) --------

    void Input::OnKey(Key key, bool down)
    {
        auto& s = GetState();
        int i = KeyIndex(key);
        if (i < 0)
            return;
        s.keys[i] = down;
    }

    void Input::OnMouseButton(MouseButton button, bool down)
    {
        auto& s = GetState();
        int i = MouseIndex(button);
        if (i < 0)
            return;
        s.mouse[i] = down;
    }

    void Input::OnMouseMove(float x, float y)
    {
        auto& s = GetState();
        s.mouseX = x;
        s.mouseY = y;
    }

    void Input::OnScroll(float yOffset)
    {
        auto& s = GetState();
        s.scrollDeltaY += yOffset;
    }

    void Input::ClearAll()
    {
        auto& s = GetState();
        std::memset(s.keys, 0, sizeof(s.keys));
        std::memset(s.prevKeys, 0, sizeof(s.prevKeys));
        std::memset(s.mouse, 0, sizeof(s.mouse));
        std::memset(s.prevMouse, 0, sizeof(s.prevMouse));
        s.scrollDeltaY = 0.0f;

        for (int i = 0; i < MaxGamepads; ++i)
        {
            // Leave connected flag unchanged — the pad is still physically connected.
            std::memset(s.gamepads[i].buttons, 0, sizeof(s.gamepads[i].buttons));
            std::memset(s.gamepads[i].prevButtons, 0, sizeof(s.gamepads[i].prevButtons));
            std::memset(s.gamepads[i].axes, 0, sizeof(s.gamepads[i].axes));
        }
    }

    // -------- Gamepad queries --------

    bool Input::IsGamepadConnected(int padId)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return false;
        return GetState().gamepads[padId].connected;
    }

    static int GamepadButtonIndex(GamepadButton button)
    {
        int v = static_cast<int>(button);
        if (v < 0 || v >= static_cast<int>(GamepadButton::Count))
            return -1;
        return v;
    }

    static int GamepadAxisIndex(GamepadAxis axis)
    {
        int v = static_cast<int>(axis);
        if (v < 0 || v >= static_cast<int>(GamepadAxis::Count))
            return -1;
        return v;
    }

    bool Input::IsGamepadButtonDown(GamepadButton button, int padId)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return false;
        int i = GamepadButtonIndex(button);
        return i >= 0 && GetState().gamepads[padId].buttons[i];
    }

    bool Input::IsGamepadButtonPressed(GamepadButton button, int padId)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return false;
        int i = GamepadButtonIndex(button);
        if (i < 0)
            return false;
        const auto& pad = GetState().gamepads[padId];
        return pad.buttons[i] && !pad.prevButtons[i];
    }

    bool Input::IsGamepadButtonReleased(GamepadButton button, int padId)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return false;
        int i = GamepadButtonIndex(button);
        if (i < 0)
            return false;
        const auto& pad = GetState().gamepads[padId];
        return !pad.buttons[i] && pad.prevButtons[i];
    }

    float Input::GetGamepadAxis(GamepadAxis axis, int padId)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return 0.0f;
        int i = GamepadAxisIndex(axis);
        return i >= 0 ? GetState().gamepads[padId].axes[i] : 0.0f;
    }

    // -------- Gamepad sinks (called by platform) --------

    void Input::OnGamepadConnected(int padId)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return;
        auto& pad = GetState().gamepads[padId];
        pad.connected = true;
        std::memset(pad.buttons, 0, sizeof(pad.buttons));
        std::memset(pad.prevButtons, 0, sizeof(pad.prevButtons));
        std::memset(pad.axes, 0, sizeof(pad.axes));
    }

    void Input::OnGamepadDisconnected(int padId)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return;
        auto& pad = GetState().gamepads[padId];
        pad.connected = false;
        std::memset(pad.buttons, 0, sizeof(pad.buttons));
        std::memset(pad.prevButtons, 0, sizeof(pad.prevButtons));
        std::memset(pad.axes, 0, sizeof(pad.axes));
    }

    void Input::OnGamepadPoll(int padId, const bool* buttons, int buttonCount,
                              const float* axes, int axisCount)
    {
        if (padId < 0 || padId >= MaxGamepads)
            return;
        auto& pad = GetState().gamepads[padId];

        int bc = std::min(buttonCount, static_cast<int>(GamepadButton::Count));
        for (int i = 0; i < bc; ++i)
            pad.buttons[i] = buttons[i];

        int ac = std::min(axisCount, static_cast<int>(GamepadAxis::Count));
        for (int i = 0; i < ac; ++i)
            pad.axes[i] = axes[i];
    }

    void Input::OnEvent(Event& e)
    {
        auto& s = GetState();
        MTS_ASSERT(s.initialized && "Input::Init must be called before Input::OnEvent.");

        EventDispatcher d(e);

        d.Dispatch<KeyPressedEvent>(
            [](KeyPressedEvent& ev)
            {
                OnKey(ev.GetKeyCode(), true);
                return false;
            });

        d.Dispatch<KeyReleasedEvent>(
            [](KeyReleasedEvent& ev)
            {
                OnKey(ev.GetKeyCode(), false);
                return false;
            });

        d.Dispatch<MouseButtonPressedEvent>(
            [](MouseButtonPressedEvent& ev)
            {
                OnMouseButton(ev.GetMouseButton(), true);
                return false;
            });

        d.Dispatch<MouseButtonReleasedEvent>(
            [](MouseButtonReleasedEvent& ev)
            {
                OnMouseButton(ev.GetMouseButton(), false);
                return false;
            });

        d.Dispatch<MouseMovedEvent>(
            [](MouseMovedEvent& ev)
            {
                OnMouseMove(ev.GetX(), ev.GetY());
                return false;
            });

        d.Dispatch<MouseScrolledEvent>(
            [](MouseScrolledEvent& ev)
            {
                OnScroll(ev.GetYOffset());
                return false;
            });

        d.Dispatch<WindowLostFocusEvent>(
            [](WindowLostFocusEvent&)
            {
                ClearAll();
                return false;
            });

        d.Dispatch<GamepadConnectedEvent>(
            [](GamepadConnectedEvent& ev)
            {
                OnGamepadConnected(ev.GetPadId());
                return false;
            });

        d.Dispatch<GamepadDisconnectedEvent>(
            [](GamepadDisconnectedEvent& ev)
            {
                OnGamepadDisconnected(ev.GetPadId());
                return false;
            });
    }

}  // namespace mts
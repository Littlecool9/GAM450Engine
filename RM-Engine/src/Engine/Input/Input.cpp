/**
 * @file Input.cpp
 * @author rahul
 * @brief // Centralized, platform-independent input service providing frame-based input queries.
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "Input.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/InputEvent.h"
#include "Engine/Events/WindowEvent.h"

namespace rm
{
    Input::State &Input::GetState()
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
        auto &s = GetState();
        s = State{};
        s.initialized = true;
        LOG_INFO("Input initialized.");
    }

    void Input::Shutdown()
    {
        auto &s = GetState();
        s = State{};
        LOG_INFO("Input shutdown.");
    }

    void Input::BeginFrame()
    {
        auto &s = GetState();
        RM_ASSERT(s.initialized && "Input::Init must be called before BeginFrame.");

        std::memcpy(s.prevKeys, s.keys, sizeof(s.keys));
        std::memcpy(s.prevMouse, s.mouse, sizeof(s.mouse));
        s.scrollDeltaY = 0.0f;
    }

    // -------- Queries --------

    bool Input::IsKeyDown(Key key)
    {
        const auto &s = GetState();
        int i = KeyIndex(key);
        return i >= 0 && s.keys[i];
    }

    bool Input::IsKeyPressed(Key key)
    {
        const auto &s = GetState();
        int i = KeyIndex(key);
        return i >= 0 && s.keys[i] && !s.prevKeys[i];
    }

    bool Input::IsKeyReleased(Key key)
    {
        const auto &s = GetState();
        int i = KeyIndex(key);
        return i >= 0 && !s.keys[i] && s.prevKeys[i];
    }

    bool Input::IsMouseDown(MouseButton button)
    {
        const auto &s = GetState();
        int i = MouseIndex(button);
        return i >= 0 && s.mouse[i];
    }

    bool Input::IsMousePressed(MouseButton button)
    {
        const auto &s = GetState();
        int i = MouseIndex(button);
        return i >= 0 && s.mouse[i] && !s.prevMouse[i];
    }

    bool Input::IsMouseReleased(MouseButton button)
    {
        const auto &s = GetState();
        int i = MouseIndex(button);
        return i >= 0 && !s.mouse[i] && s.prevMouse[i];
    }

    float Input::GetMouseX() { return GetState().mouseX; }
    float Input::GetMouseY() { return GetState().mouseY; }

    std::pair<float, float> Input::GetMousePosition()
    {
        const auto &s = GetState();
        return {s.mouseX, s.mouseY};
    }

    float Input::GetScrollDeltaY() { return GetState().scrollDeltaY; }

    // -------- Event sinks (called by platform) --------

    void Input::OnKey(Key key, bool down)
    {
        auto &s = GetState();
        int i = KeyIndex(key);
        if (i < 0)
            return;
        s.keys[i] = down;
    }

    void Input::OnMouseButton(MouseButton button, bool down)
    {
        auto &s = GetState();
        int i = MouseIndex(button);
        if (i < 0)
            return;
        s.mouse[i] = down;
    }

    void Input::OnMouseMove(float x, float y)
    {
        auto &s = GetState();
        s.mouseX = x;
        s.mouseY = y;
    }

    void Input::OnScroll(float yOffset)
    {
        auto &s = GetState();
        s.scrollDeltaY += yOffset;
    }

    void Input::ClearAll()
    {
        auto &s = GetState();
        std::memset(s.keys, 0, sizeof(s.keys));
        std::memset(s.prevKeys, 0, sizeof(s.prevKeys));
        std::memset(s.mouse, 0, sizeof(s.mouse));
        std::memset(s.prevMouse, 0, sizeof(s.prevMouse));
        s.scrollDeltaY = 0.0f;
    }

    void Input::OnEvent(Event &e)
    {
        auto &s = GetState();
        RM_ASSERT(s.initialized && "Input::Init must be called before Input::OnEvent.");

        EventDispatcher d(e);

        d.Dispatch<KeyPressedEvent>([](KeyPressedEvent &ev)
                                    {
                OnKey(ev.GetKeyCode(), true);
                return false; });

        d.Dispatch<KeyReleasedEvent>([](KeyReleasedEvent &ev)
                                     {
                OnKey(ev.GetKeyCode(), false);
                return false; });

        d.Dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent &ev)
                                            {
                OnMouseButton(ev.GetMouseButton(), true);
                return false; });

        d.Dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent &ev)
                                             {
                OnMouseButton(ev.GetMouseButton(), false);
                return false; });

        d.Dispatch<MouseMovedEvent>([](MouseMovedEvent &ev)
                                    {
                OnMouseMove(ev.GetX(), ev.GetY());
                return false; });

        d.Dispatch<MouseScrolledEvent>([](MouseScrolledEvent &ev)
                                       {
                OnScroll(ev.GetYOffset());
                return false; });

        d.Dispatch<WindowLostFocusEvent>([](WindowLostFocusEvent &)
                                         {
                ClearAll();
                return false; });
    }

} // rm namespace
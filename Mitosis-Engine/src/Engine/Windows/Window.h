/**
 * @file Window.h
 * @author sumin.park
 * @brief The interface representing the window.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <memory>
#include <string>
#include "Engine/Events/Event.h"

namespace mts
{
    struct WindowProps
    {
        std::string Title = "Mitosis-Engine";
        uint32_t Width = 1600;
        uint32_t Height = 900;
        bool VSync = false;
        std::string IconPath = "";
    };

    class Window
    {
       public:
        virtual ~Window() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual bool GetVsync() const = 0;

        virtual void SetVsync(bool enable) = 0;
        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual bool IsFullscreen() const = 0;

        // TODO : Move this to Renderer if we ever have one later
        virtual void ClearScreen() = 0;

        virtual void PollEvents() = 0;

        // TODO : Move this to a swapchain class
        virtual void SwapBuffer() = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual void* NativeWindow() const = 0;

        static std::unique_ptr<Window> Create(const WindowProps& props = {});

       protected:
        Window() = default;
    };

}  // namespace mts
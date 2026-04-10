/**
 * @file WinGLFWWindow.h
 * @author sumin.park
 * @brief  Window class for the Windows OS
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include "Window.h"
struct GLFWwindow;

namespace mts
{
    class WinGLFWwindow final : public Window
    {
       public:
        WinGLFWwindow(const WindowProps& props);
        ~WinGLFWwindow() override;

        uint32_t GetWidth() const override { return windowData.Width; }
        uint32_t GetHeight() const override { return windowData.Height; }
        bool GetVsync() const override { return windowData.VSync; }

        void SetVsync(bool enable) override;
        void SetFullscreen(bool fullscreen) override;
        bool IsFullscreen() const override { return mIsFullscreen; }

        void ClearScreen() override;

        virtual void PollEvents() override;

        virtual void SwapBuffer() override;

        void SetEventCallback(const EventCallbackFn& callback) override;


        void* NativeWindow() const override { return window; }

       private:
        void Init(const WindowProps& props);
        void Shutdown();

        struct WindowData
        {
            std::string Title;
            int Width = 0;
            int Height = 0;
            bool VSync = true;

            EventCallbackFn EventCallback;
        };

        WindowData windowData;
        GLFWwindow* window = nullptr;

        bool mIsFullscreen   = false;
        int  mWindowedX      = 100, mWindowedY      = 100;
        int  mWindowedWidth  = 1600, mWindowedHeight = 900;
    };

}  // namespace mts
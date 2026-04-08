/**
 * @file AppSystem.h
 * @author sumin.park
 * @brief  Script-facing application lifecycle interface
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Events/Event.h"
namespace mts
{
    class AppSystem
    {
       public:
        static AppSystem& Get();

        /// Request the application to quit.
        void Quit();
        /// Request the application to toggle fullscreen/windowed mode.
        void ToggleFullscreen();
        void SetEventCallback(const EventCallbackFn& callback) { sEventCallback = callback; }

       private:
        EventCallbackFn sEventCallback;
    };

}  // namespace mts

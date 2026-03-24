/**
 * @file AppSystem.h
 * @author sumin.park
 * @brief  Script-facing application lifecycle interface
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class AppSystem
    {
       public:
        static AppSystem& Get();

        /// Request the application to quit. Fires onQuitRequested synchronously.
        void Quit();

        /// Subscribe to be notified when Quit() is called.
        /// Application subscribes internally to set mIsRunning = false.
        Delegate<> onQuitRequested;
    };

}  // namespace mts

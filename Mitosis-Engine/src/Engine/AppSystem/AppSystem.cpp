/**
 * @file AppSystem.cpp
 * @author sumin.park
 * @brief  Script-facing application lifecycle interface
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AppSystem.h"

#include "Engine/Events/AppEvent.h"

namespace mts
{
    AppSystem& AppSystem::Get()
    {
        static AppSystem instance;
        return instance;
    }

    void AppSystem::Quit()
    {
        if (sEventCallback)
        {
            AppQuitRequestEvent e;
            sEventCallback(e);
        }
    }

    void AppSystem::ToggleFullscreen()
    {
        if (sEventCallback)
        {
            AppToggleFullscreenRequestEvent e;
            sEventCallback(e);
        }
    }

}  // namespace mts

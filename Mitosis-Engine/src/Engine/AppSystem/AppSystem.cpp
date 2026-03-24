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

namespace mts
{
    AppSystem& AppSystem::Get()
    {
        static AppSystem instance;
        return instance;
    }

    void AppSystem::Quit()
    {
        onQuitRequested.Invoke();
    }

}  // namespace mts

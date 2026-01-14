/**
 * @file Component.cpp
 * @author sumin.park
 * @brief  Basic Component class
 * @version 0.1
 * @date 1/8/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Component.h"

namespace mts
{
    Component::Component(GameObject* owner) : owner(owner)
    {
    }
}  // namespace mts
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
    Component::Component() : owner(nullptr), isDeserialized(false), isEnabled(true)
    {
    }
    Component::Component(GameObject* owner) : owner(owner), isDeserialized(false), isEnabled(true)
    {
    }

    void Component::Deserialize(JsonValue const& jsonObj)
    {
        if (isDeserialized)
            return;
        // Jackey TODO: implement component specific deserialization here
        isDeserialized = true;
    }

    void Component::SetEnabled(bool enabled)
    {
        if (isEnabled == enabled)
            return;
        isEnabled = enabled;
        if (isEnabled)
        {
            onEnable.Invoke();
        }
        else
        {
            onDisable.Invoke();
        }
    }
}  // namespace mts
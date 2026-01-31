/**
 * @file Collider2D.cpp
 * @author sumin.park
 * @brief  Base 2D Collider component.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Collider2D.h"

namespace mts
{
    Collider2D::~Collider2D()
    {
    }

    void Collider2D::Serialize(JsonDocument& doc) const
    {
    }
    void Collider2D::Deserialize(const JsonValue& jsonObj)
    {
        Component::Deserialize(jsonObj);
    }
}  // namespace mts
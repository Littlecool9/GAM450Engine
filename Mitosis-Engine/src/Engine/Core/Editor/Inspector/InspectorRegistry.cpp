/**
 * @file InspectorRegistry.cpp
 * @author dream
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "InspectorRegistry.h"

namespace mts
{
    void InspectorRegistry::DrawAll(GameObject* go)
    {
        for (auto& d : mDrawers)
            d->Draw(go);
    }
}  // namespace mts
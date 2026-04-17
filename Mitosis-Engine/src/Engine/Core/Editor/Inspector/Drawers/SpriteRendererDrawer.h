/**
 * @file SpriteRendererDrawer.h
 * @author sumin.park
 * @brief  drawer for sprite renderer
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class SpriteRendererDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "SpriteRenderer"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts
/**
 * @file CircleCollider2DDrawer.h
 * @author sumin.park
 * @brief  drawer for circlecollider2d
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class CircleCollider2DDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "CircleCollider2D"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts
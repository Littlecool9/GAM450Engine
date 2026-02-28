/**
 * @file TransformDrawer.h
 * @author sumin.park
 * @brief  drawer for transform
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class TransformDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "Transform"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts
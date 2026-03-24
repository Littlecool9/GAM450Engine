/**
 * @file UITransformDrawer.h
 * @author sumin.park
 * @brief  drawer for UITransform Component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"
namespace mts
{
    class UITransformDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "UI Transform"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

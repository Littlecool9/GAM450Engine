/**
 * @file ActionListDrawer.h
 * @author sumin.park
 * @brief drawer for ActionList component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class ActionListDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "ActionList"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

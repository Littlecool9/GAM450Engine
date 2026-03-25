/**
 * @file ButtonDrawer.h
 * @author Dong Ta
 * @brief  Inspector drawer for the Button component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class ButtonDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "Button"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

/**
 * @file NativeScriptDrawer.h
 * @author sumin.park
 * @brief  drawer for native scripts
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class NativeScriptDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "NativeScript"; }
        void Draw(GameObject* go) override;
    };

}  // namespace mts

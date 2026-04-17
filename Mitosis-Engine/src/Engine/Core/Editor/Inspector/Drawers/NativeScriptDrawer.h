/**
 * @file NativeScriptDrawer.h
 * @author sumin.park
 * @brief  drawer for native scripts
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
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
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

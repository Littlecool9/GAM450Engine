/**
 * @file TilemapDrawer.h
 * @author sumim.park
 * @brief  drawer for tilemap component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class TilemapDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "Tilemap"; }
        bool Draw(GameObject* go) override;

       private:
    };

}  // namespace mts
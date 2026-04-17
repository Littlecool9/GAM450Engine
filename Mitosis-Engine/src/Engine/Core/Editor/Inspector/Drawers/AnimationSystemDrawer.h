/**
 * @file AnimationSystemDrawer.h
 * @author sumin.park
 * @brief drawer for AnimationSystem component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class AnimationSystemDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "AnimationSystem"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

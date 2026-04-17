/**
 * @file AnimationDrawer.h
 * @author sumin.park
 * @brief drawer for Animation component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class AnimationDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "Animation"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

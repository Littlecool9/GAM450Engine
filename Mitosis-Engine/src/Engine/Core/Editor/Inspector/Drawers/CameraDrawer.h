/**
 * @file CameraDrawer.h
 * @author sumin.park
 * @brief drawer for CameraComponent
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class CameraDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "Camera"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

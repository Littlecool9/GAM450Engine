/**
 * @file CameraFollowDrawer.h
 * @author sumin.park
 * @brief drawer for CameraFollow component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class CameraFollowDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "CameraFollow"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

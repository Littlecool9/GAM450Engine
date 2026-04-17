/**
 * @file AudioListenerDrawer.h
 * @author sumin.park
 * @brief drawer for AudioListener component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class AudioListenerDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "AudioListener"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

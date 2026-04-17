/**
 * @file AudioEmitterDrawer.h
 * @author sumin.park
 * @brief drawer for AudioEmitter component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class AudioEmitterDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "AudioEmitter"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

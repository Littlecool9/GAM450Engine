/**
 * @file ParticleEmitterDrawer.h
 * @author sumin.park
 * @brief drawer for ParticleEmitter component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Core/Editor/Inspector/ComponentDrawer.h"

namespace mts
{
    class ParticleEmitterDrawer : public ComponentDrawer
    {
       public:
        const char* GetName() const override { return "ParticleEmitter"; }
        bool Draw(GameObject* go) override;
    };

}  // namespace mts

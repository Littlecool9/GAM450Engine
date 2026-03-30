/**
 * @file PlayerParticleController.cpp
 * @author josh.siegel
 * @brief // Handles spawning and moving particle objects for the player
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PlayerParticleController.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/Particles/ParticleEmitter.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"

PlayerParticleController::PlayerParticleController()
{
    chargeParticlesObject = mts::GameObjectFactory::CreateFromArchetype("Data/ChargeParticles.prefab");
    chargeParticlesObject->SetActive(false);
    chargeParticles = chargeParticlesObject->GetComponent<mts::ParticleEmitter>();
}

PlayerParticleController::~PlayerParticleController()
{
    if (chargeParticlesObject)
        chargeParticlesObject->Destroy();
    chargeParticlesObject = nullptr;
}

// Player calls this with its position each frame to keep
//  local position particle emitters at the player's position
void PlayerParticleController::UpdateParticlePositions(glm::vec2 pos)
{
    if (chargeParticlesObject)
    {
        mts::Transform* transform = chargeParticlesObject->GetComponent<mts::Transform>();
        if (transform)
            transform->SetPosition(pos);
    }
}

// Starts a particle emitter of given type
void PlayerParticleController::StartParticleEmitter(ParticleType type)
{
    switch (type)
    {
        case CHARGE:
            chargeParticlesObject->SetActive(true);
            if (chargeParticles)
                chargeParticles->ToggleEmitter();
            break;
        default:
            break;
    }
}

// Stops a particle emitter of given type
void PlayerParticleController::StopParticleEmitter(ParticleType type)
{
    switch (type)
    {
        case CHARGE:
            chargeParticlesObject->SetActive(false);
            if (chargeParticles)
                chargeParticles->ToggleEmitter();
            break;
        default:
            break;
    }
}

void PlayerParticleController::SpawnParticleEmitter(ParticleType type, glm::vec2 pos)
{
    mts::Transform* transform = nullptr;
    mts::GameObject* particleObject = nullptr;

    switch (type)
    {
        case ON_BEAT_LIGHT:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OnBeatLightParticles.prefab");
            transform = particleObject->GetComponent<mts::Transform>();
            break;
        case OFF_BEAT_LIGHT:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OffBeatLightParticles.prefab");
            transform = particleObject->GetComponent<mts::Transform>();
            break;
        case ON_BEAT_HEAVY:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OnBeatHeavyParticles.prefab");
            transform = particleObject->GetComponent<mts::Transform>();
            break;
        case OFF_BEAT_HEAVY:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OffBeatHeavyParticles.prefab");
            transform = particleObject->GetComponent<mts::Transform>();
            break;
        default:
            break;
    }

    if (transform)
        transform->SetPosition(pos);
}

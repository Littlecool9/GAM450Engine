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
    
}

PlayerParticleController::~PlayerParticleController()
{

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
    if (!chargeParticlesObject)
    {
        chargeParticlesObject =
            mts::GameObjectFactory::CreateFromArchetype("Data/ChargeParticles.prefab");
        if (chargeParticlesObject)
        {
            chargeParticles = chargeParticlesObject->GetComponent<mts::ParticleEmitter>();
        }
    }

    switch (type)
    {
        case CHARGE:
            if (chargeParticles)
                chargeParticles->StartEmitter();
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
            if (chargeParticles)
                chargeParticles->StopEmitter();
            break;
        default:
            break;
    }
}

void PlayerParticleController::SpawnParticleEmitter(ParticleType type,
                                                    glm::vec2 posParticle,
                                                    ParticleTiming timing,
                                                    glm::vec2 posTiming)
{
    mts::Transform* transform = nullptr;
    mts::GameObject* particleObject = nullptr;

    switch (type)
    {
        case ON_BEAT_LIGHT:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OnBeatLightParticles.prefab");
            break;
        case OFF_BEAT_LIGHT:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OffBeatLightParticles.prefab");
            break;
        case ON_BEAT_HEAVY:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OnBeatHeavyParticles.prefab");
            break;
        case OFF_BEAT_HEAVY:
            particleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/OffBeatHeavyParticles.prefab");
            break;
        default:
            break;
    }
    if (particleObject)
        transform = particleObject->GetComponent<mts::Transform>();
    if (transform)
        transform->SetPosition(posParticle);

    mts::Transform* timingTransform = nullptr;
    mts::GameObject* timingParticleObject = nullptr;
    switch (timing)
    {
        case EARLY:
            timingParticleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/EarlyHitParticle.prefab");
            break;
        case PERFECT:
            timingParticleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/PerfectHitParticle.prefab");
            break;
        case LATE:
            timingParticleObject =
                mts::GameObjectFactory::CreateFromArchetype("Data/LateHitParticle.prefab");
            break;
        default:
            break;
    }
    if (timingParticleObject)
        timingTransform = timingParticleObject->GetComponent<mts::Transform>();
    if (timingTransform)
        timingTransform->SetPosition(posTiming);
}

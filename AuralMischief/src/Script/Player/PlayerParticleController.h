/**
 * @file PlayerParticleController.h
 * @author josh.siegel
 * @brief // Handles spawning and moving particle objects for the player
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/Particles/ParticleEmitter.h"
class PlayerParticleController
{
public:
    enum ParticleType
    {
        CHARGE = 0,
        FULL_CHARGE,
        ON_BEAT_LIGHT,
        OFF_BEAT_LIGHT,
        ON_BEAT_HEAVY,
        OFF_BEAT_HEAVY,
        JUMP,
        DASH
    };

    enum ParticleTiming
    {
        NONE = 0,
        EARLY,
        PERFECT,
        LATE
    };

    PlayerParticleController();
    ~PlayerParticleController();

    // Player calls this with its position each frame to keep
    //  local position particle emitters at the player's position
    void UpdateParticlePositions(glm::vec2 pos);

    // Starts a particle emitter (follows player, not one-shot) of given type
    void StartParticleEmitter(ParticleType type);

    // Stops a particle emitter (follows player, not one-shot) of given type
    void StopParticleEmitter(ParticleType type);

    // Spawns a one-shot particle emitter at a given position
    void SpawnParticleEmitter(ParticleType type,
                              glm::vec2 posParticle,
                              ParticleTiming timing,
                              glm::vec2 posTiming);

private:

    // Particle emitter objects that are attached to player (not one-shot)
    mts::GameObject* chargeParticlesObject = nullptr;
    mts::ParticleEmitter* chargeParticles = nullptr;
};

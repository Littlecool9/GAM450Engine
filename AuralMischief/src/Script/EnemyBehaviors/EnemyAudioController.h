/**
 * @file EnemyAudioController.h
 * @author dream
 * @brief  Shared service script that plays type-appropriate SFX for all enemy types
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Audio/AudioTypes.h"
#include "EnemyController.h"

#include <array>
#include <unordered_map>

class EnemyAudioController : public mts::ScriptBase
{
public:
    void Init() override;

    void PlayHitSound(mts::EnemyType type);
    void PlayAlertSound(mts::EnemyType type);  // no-op for FlyEnemy (no alert sounds exist)
    mts::AudioHandle PlaySlugBeep();
    void PlaySlugExplode(mts::AudioHandle fuseHandle);
    void PlayFlyExplode();
    void PlayBossMoan();
    SCRIPT_CLASS(EnemyAudioController)

private:
    std::unordered_map<int, std::array<const char*, 3>> mHitSounds;
    std::unordered_map<int, std::array<const char*, 3>> mAlertSounds;
};

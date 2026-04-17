/**
 * @file PlayerAudioController.cpp
 * @author josh.siegel
 * @brief  script for player audio controller
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "PlayerAudioController.h"
#include "Engine/Audio/AudioManager.h"

void PlayerAudioController::Init()
{
    soundMap.insert(std::pair(ATTACK1, "event:/guitar_attack_01"));
    soundMap.insert(std::pair(ATTACK2, "event:/guitar_attack_02"));
    soundMap.insert(std::pair(ATTACK3, "event:/guitar_attack_03"));
    soundMap.insert(std::pair(ATTACK4, "event:/guitar_attack_04"));
    soundMap.insert(std::pair(ATTACK5, "event:/guitar_attack_05"));
    soundMap.insert(std::pair(DASH, "event:/hero_dash"));
    soundMap.insert(std::pair(DASH2, "event:/hero_dash_vibration"));
    soundMap.insert(std::pair(EVADE, "event:/hero_evade"));
    soundMap.insert(std::pair(FALL, "event:/hero_falling"));
    soundMap.insert(std::pair(CHARGE_START, "event:/hero_guitar_charge_initiate"));
    soundMap.insert(std::pair(CHARGE_START2, "event:/hero_guitar_charge_initiate_vibration"));
    soundMap.insert(std::pair(CHARGE_COMPLETE, "event:/hero_guitar_charge_complete"));
    soundMap.insert(std::pair(CHARGE_COMPLETE2, "event:/hero_guitar_charge_complete_vibration"));
    soundMap.insert(std::pair(CHARGE_LOOP, "event:/hero_guitar_charge_loop"));
    soundMap.insert(std::pair(LAND_HARD, "event:/hero_land_hard"));
    soundMap.insert(std::pair(LAND_SOFT, "event:/hero_land_soft"));
    soundMap.insert(std::pair(JUMP, "event:/hero_wall_jump"));
    soundMap.insert(std::pair(DAMAGE, "event:/player_damaged"));
}

void PlayerAudioController::Awake()
{
}

void PlayerAudioController::OnDisable()
{
}

void PlayerAudioController::DeserializeFields(mts::JsonValue const& fields)
{
}

void PlayerAudioController::SerializeFields(mts::JsonDocument& out) const
{
}

void PlayerAudioController::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
}

void PlayerAudioController::PlayOneShotSound(SFX sound)
{
    auto it = soundMap.find(sound);
    if (it != soundMap.end())
    {
        auto handle = mts::AudioManager::Get().Play(it->second);
    }
}
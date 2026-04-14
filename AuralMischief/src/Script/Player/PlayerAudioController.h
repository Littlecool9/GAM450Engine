/**
 * @file PlayerAudioController.h
 * @author josh.siegel
 * @brief  script for player audio controller
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class PlayerAudioController : public mts::ScriptBase
{
   public:
       enum SFX
       {
           ATTACK1 = 0,
           ATTACK2,
           ATTACK3,
           ATTACK4,
           ATTACK5,
           DASH,
           DASH2,
           EVADE,
           FALL,
           CHARGE_START,
           CHARGE_START2,
           CHARGE_COMPLETE,
           CHARGE_COMPLETE2,
           CHARGE_LOOP,
           LAND_HARD,
           LAND_SOFT,
           JUMP,
           DAMAGE
       };

    void Init() override;
    void Awake() override;
    void OnDisable() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    SCRIPT_CLASS(PlayerAudioController)

    void PlayOneShotSound(SFX sound);

   private:
    std::unordered_map<SFX, const char*> soundMap;
};
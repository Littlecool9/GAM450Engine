/**
 * @file AttackHitboxScript.h
 * @author josh.siegel
 * @brief // Script to control attack hitboxes spawned by player
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class AttackHitboxScript : public mts::ScriptBase
{
   public:
	void OnCreate() override;
	void OnUpdate(float) override;
	void DeserializeFields(mts::JsonValue const&) override;
	void SerializeFields(mts::JsonDocument& out) const override;

	float GetDamage() const { return mDamage; }
    void SetDamage(float damage) { mDamage = damage; }

	SCRIPT_CLASS(AttackHitboxScript)
   private:
    float mLifetime = 0.5f;
    float mDamage = 1.0f;

	float lifeTimer = -1.0f;
};
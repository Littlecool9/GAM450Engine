/**
 * @file AttackHitboxScript.h
 * @author josh.siegel
 * @brief // Script to control attack hitboxes spawned by player
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class AttackHitboxScript : public mts::ScriptBase
{

   public:
    enum class Team
    {
        Player,
        Enemy
    };
	void Init() override;
	void OnEnable() override;
	void OnUpdate(float) override;
	void DeserializeFields(mts::JsonValue const&) override;
	void SerializeFields(mts::JsonDocument& out) const override;

	float GetDamage() const { return mDamage; }
    void SetDamage(float damage) { mDamage = damage; }
    void SetLifetime(float lifetime) { mLifetime = lifetime; }
    glm::vec2 GetDirection() const { return direction; }
    void SetDirection(glm::vec2 dir) { direction = dir; }
	Team GetTeam() const { return mTeam; }
    void SetTeam(Team team) { mTeam = team; }

	SCRIPT_CLASS(AttackHitboxScript)
   private:
    float mLifetime = 0.5f;
    float mDamage = 1.0f;
    Team mTeam = Team::Player;
	float lifeTimer = -1.0f;
    glm::vec2 direction{0.0f, 0.0f};
};
/**
 * @file EnemyAttackAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyAttackAction.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
namespace mts
{
	EnemyState EnemyAttackAction::GetExpectedNextState(const EnemyState& state,
		const EnemyPreference& preferences) const
	{
		return EnemyState();
    }

	float EnemyAttackAction::GetEstimatedTimeCost(const EnemyState& state,
		const EnemyPreference& preferences) const
	{
		return 0.0f;
    }

	void EnemyAttackAction::OnBeginning(const EnemyAgent& agent)
	{
    }
    
	bool EnemyAttackAction::IsAvailable(const EnemyState& state, 
		const EnemyPreference& preferences) const
	{
		return false;
    }

	EnemyActionResult EnemyAttackAction::Update(const EnemyAgent agent,
                                                float dt,
                                                float totalTime) const
	{
        return EnemyActionResult::Invalid;
	}

	void EnemyAttackAction::OnEnd(const EnemyAgent& agent, EnemyActionResult result)
	{
    }

	std::unordered_map<EnemyGoal, float> EnemyAttackAction::InternalHeuristic(
	const EnemyState& state,
        const EnemyPreference& preference)
	{
        return std::unordered_map<EnemyGoal, float>();
	}

}  // namespace mts
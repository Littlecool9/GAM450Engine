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

#include <fmod_dsp_effects.h>

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/Animation.h"

namespace mts
{
	EnemyState EnemyAttackAction::GetExpectedNextState(const EnemyState& state,
		const EnemyPreference& preferences) const
	{
        EnemyState newState = state;
		//TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.6f);

		return newState;
    }

	float EnemyAttackAction::GetEstimatedTimeCost(const EnemyState& state,
		const EnemyPreference& preferences) const
	{
		return 1.28f;
    }

    void EnemyAttackAction::FireBullet(EnemyAgent& agent)
    {
        //++mBulletsFired;
        //LOG_INFO("Bullet fired {}", mBulletsFired);  // spawn bullet
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack1);
    }

	void EnemyAttackAction::OnBeginning(EnemyAgent& agent)
	{
        agent.GetState().SetInAction(true);
        mBulletsFired = 0;
        glm::vec3 dir = agent.GetPlayerReference()->GetComponent<mts::Transform>()->GetPosition() - agent.GetGameObject()->GetComponent<Transform>()->GetPosition();

        ConductorSystem& cs = ConductorSystem::Get();
        mConductorSubId = cs.onBeat.Subscribe(
            [this, &agent](mts::BeatInfo info)
            {
                if (info.isHalfBeat)
                    FireBullet(agent);
        });
        // debug
        LOG_INFO("EnemyAttackAction: Attacking player! Resetted bulletcount: {}", mBulletsFired);
    }
    
	bool EnemyAttackAction::IsAvailable(const EnemyState& state, 
		const EnemyPreference& preferences) const
	{
        if (state.GetInAction() || state.GetAttackCountdown() > 0)
        {
            return false;
        }
        return true;
    }

	EnemyActionResult EnemyAttackAction::Update(EnemyAgent* agent,
                                                float dt,
                                                float totalTime) 
	{
        if (!agent)
        {
            return Invalid;
        }

        if (agent->GetGameObject()->GetComponent<Animation>()->AnimationIsDone())
        {
            ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            agent->GetState().SetAttackCountdown(agent->GetState().GetAttackCooldownTime());  // cooldown after attack
            agent->GetState().SetInAction(false);
            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Idle);
            LOG_INFO("Finished Firing");
            return Success;
        }
        else
        {
            return OnGoing;
        }

	}

	void EnemyAttackAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
	{
        if (result == Failed)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

	std::unordered_map<EnemyGoal, float> EnemyAttackAction::InternalHeuristic(
	const EnemyState& state,
        const EnemyPreference& preference)
	{
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
	}

}  // namespace mts
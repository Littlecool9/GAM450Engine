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

#include "../Actions/ColliderScaleAction.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "params/AttackCountdownParam.h"

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
        if (!agent.GetGameObject())
        {
            return;
        }
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack1);
        auto a = agent.GetGameObject()->GetComponent<Animation>()->GetFrameDuration();
        glm::vec2 newSize{size.x * 3.3, size.y};
        LOG_INFO("Scaling up");
        agent.GetGameObject()->GetComponent<ActionList>()->AddAction(new 
            ColliderScaleAction(newSize, agent.GetGameObject(), 1, a * 1.f, a * 5, true, BaseAction::EaseType::Linear));
        agent.GetGameObject()->GetComponent<ActionList>()->AddAction(
            new ColliderScaleAction(size,
                                    agent.GetGameObject(),
                                    1,
                                    a,
                                    a,
                                    false,
                                    BaseAction::EaseType::Linear));
        ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
    }

	void EnemyAttackAction::OnBeginning(EnemyAgent& agent)
	{
        agent.GetState().SetInAction(true);
        glm::vec3 dir = agent.GetPlayerReference()->GetComponent<mts::Transform>()->GetPosition() - agent.GetGameObject()->GetComponent<Transform>()->GetPosition();
        size = agent.GetGameObject()->GetComponent<BoxCollider2D>()->GetLocalSize();
        ConductorSystem& cs = ConductorSystem::Get();
        mConductorSubId = cs.onBeat.Subscribe(
            [this, &agent](mts::BeatInfo info)
            {
                if (info.isHalfBeat)
                    FireBullet(agent);
        });

        // debug
        LOG_INFO("EnemyAttackAction: Attacking player!",);
    }
    
	bool EnemyAttackAction::IsAvailable(const EnemyState& state, 
		const EnemyPreference& preferences) const
	{
        if (state.GetInAction() ||
            std::get<float>(state.params.at(HashAction("AttackCooldown"))->GetValue()) > 0)
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
             // cooldown after attack
            agent->GetState()
                .params.at(HashAction("AttackCooldown"))
                ->SetValue(static_cast<AttackCountdownParam*>(
                              agent->GetState().params.at(HashAction("AttackCooldown")))
                              ->GetCooldownTime());
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
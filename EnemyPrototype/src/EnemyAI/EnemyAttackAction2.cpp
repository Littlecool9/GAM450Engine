/**
 * @file EnemyAttackAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyAttackAction2.h"

#include <fmod_dsp_effects.h>

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/Actions/BaseAction.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "../Actions/ColliderScaleAction.h"
#include "params/AttackCountdown2Param.h"

namespace mts
{

    EnemyState EnemyAttackAction2::GetExpectedNextState(const EnemyState& state,
                                                        const EnemyPreference& preferences) const
	{
        EnemyState newState = state;
		//TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.4f);
		return newState;
    }

	float EnemyAttackAction2::GetEstimatedTimeCost(const EnemyState& state,
		const EnemyPreference& preferences) const
	{
		return 1.72f;
    }

    void EnemyAttackAction2::FireBullet(EnemyAgent& agent)
    {
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack2);
        auto a = agent.GetGameObject()->GetComponent<Animation>()->GetFrameDuration();
        glm::vec2 newSize{size.x * 3.6, size.y};
        LOG_INFO("Scaling up");
        agent.GetGameObject()->GetComponent<ActionList>()->AddAction(
            new ColliderScaleAction(newSize,
                                    agent.GetGameObject(),
                                    1,
                                    a,
                                    a,
                                    true,
                                    BaseAction::EaseType::Linear));
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

	void EnemyAttackAction2::OnBeginning(EnemyAgent& agent)
	{
        agent.GetState().SetInAction(true);

        size = agent.GetGameObject()->GetComponent<BoxCollider2D>()->GetLocalSize();
        glm::vec3 dir = agent.GetPlayerReference()->GetComponent<mts::Transform>()->GetPosition() - agent.GetGameObject()->GetComponent<Transform>()->GetPosition();

        ConductorSystem& cs = ConductorSystem::Get();
        mConductorSubId = cs.onBeat.Subscribe(
            [this, &agent](mts::BeatInfo info)
            {
                if (info.isHalfBeat)
                    FireBullet(agent);
        });
    }
    
	bool EnemyAttackAction2::IsAvailable(const EnemyState& state, 
		const EnemyPreference& preferences) const
	{
        if (state.GetInAction() ||
            std::get<float>(state.params.at(HashAction("Attack2Cooldown"))->GetValue()) > 0)
        {
            return false;
        }
        return true;
    }

	EnemyActionResult EnemyAttackAction2::Update(EnemyAgent* agent,
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
                .params.at(HashAction("Attack2Cooldown"))
                ->SetValue(static_cast<AttackCountdown2Param*>(
                               agent->GetState().params.at(HashAction("Attack2Cooldown")))
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

	void EnemyAttackAction2::OnEnd(EnemyAgent& agent, EnemyActionResult result)
	{
        if (result == Failed)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            // agent.GetState().SetInAction(false);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

	std::unordered_map<EnemyGoal, float> EnemyAttackAction2::InternalHeuristic(
	const EnemyState& state,
        const EnemyPreference& preference)
	{
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
	}

}  // namespace mts
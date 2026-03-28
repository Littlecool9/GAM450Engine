/**
 * @file EnemyAttackAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "FlyingEnemyAttackAction.h"
#include <Engine/EntitySystem/Component/Actions/TranslateAction.h>
#include <Engine/EntitySystem/Component/Actions/RotateAction.h>
#include <fmod_dsp_effects.h>
#include "../EnemyParameters/AttackCountdownParam.h"
//#include "../Actions/ColliderScaleAction.h"
#include "../EnemyController.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"


namespace mts
{

    EnemyState FlyingEnemyAttackAction::GetExpectedNextState(const EnemyState& state,
                                                             const EnemyPreference& preferences) const
	{
        EnemyState newState = state;
		//TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.3f);
		return newState;
    }

	float FlyingEnemyAttackAction::GetEstimatedTimeCost(const EnemyState& state,
		const EnemyPreference& preferences) const
	{
		return 2.f;
    }

    void FlyingEnemyAttackAction::FireBullet(EnemyAgent& agent)
    {
        if (!agent.GetGameObject())
        {
            return;
        }

        const glm::vec2 enemyPos = agent.GetGameObject()->GetComponent<Transform>()->GetPosition();
        const glm::vec2 playerPos =
            agent.GetPlayerReference()->GetComponent<Transform>()->GetPosition();

        const glm::vec2 toPlayer = playerPos - enemyPos;
        const float dist = agent.GetState().GetDistanceToPlayer();
        glm::vec2 dir = (dist > 0.0001f) ? (toPlayer / dist) : glm::vec2{0.0f, 0.0f};
        float modifier = dir.x < 0 ? -1 : 1;
        float first = 0.2f;
        glm::vec3 pos{enemyPos.x + first * modifier,
                  enemyPos.y,
                  agent.GetGameObject()->GetComponent<Transform>()->GetPosition().z};
        agent.GetGameObject()->GetComponent<ActionList>()->AddTranslateAction(pos, agent.GetGameObject(), 0.1f, 0, 0);
        agent.GetGameObject()->GetComponent<ActionList>()->AddRotateAction(3.665f,
                                                                           agent.GetGameObject(),
                                                                           0.25f,
                                                                           0,
                                                                           0,
                                                                           true);
        pos.x = enemyPos.x + 0.5f * modifier;
        pos.y = enemyPos.y - 3;
        agent.GetGameObject()->GetComponent<ActionList>()->AddTranslateAction(pos,
                                                                              agent.GetGameObject(),
                                                                              0.7f,
                                                                              0,
                                                                              0, true);
        pos.x = enemyPos.x;
        pos.y = enemyPos.y;
        agent.GetGameObject()->GetComponent<ActionList>()->AddTranslateAction(pos,
                                                                              agent.GetGameObject(),
                                                                              0.5f,
                                                                              0,
                                                                              0);
        agent.GetGameObject()->GetComponent<ActionList>()->AddRotateAction(0,
                                                                           agent.GetGameObject(),
                                                                           0.5f,
                                                                           0,
                                                                           0,
                                                                           true);

        LOG_INFO("Scaling up");
        ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
    }

	void FlyingEnemyAttackAction::OnBeginning(EnemyAgent& agent)
	{
        agent.GetState().SetInAction(true);
        glm::vec3 dir = agent.GetPlayerReference()->GetComponent<mts::Transform>()->GetPosition() - agent.GetGameObject()->GetComponent<Transform>()->GetPosition();

        ConductorSystem& cs = ConductorSystem::Get();

        FireBullet(agent);

        LOG_INFO("EnemyAttackAction: Attacking player!",);
    }
    
	bool FlyingEnemyAttackAction::IsAvailable(const EnemyState& state, 
		const EnemyPreference& preferences) const
	{
        float cd = std::get<float>(state.params.at(HashAction("AttackCooldown"))->GetValue());
        if (state.GetInAction() || cd > 0)
        {
            return false;
        }
        return true;
    }

	EnemyActionResult FlyingEnemyAttackAction::Update(EnemyAgent* agent,
                                                float dt,
                                                float totalTime) 
	{
        if (!agent)
        {
            return Invalid;
        }

        if (totalTime >=  std::get<float>(agent->GetState().params.at(HashAction("AttackCooldown"))->GetValue()))
        {
             // cooldown after attack
            agent->GetState()
                .params.at(HashAction("AttackCooldown"))
                ->SetValue(static_cast<AttackCountdownParam*>(
                              agent->GetState().params.at(HashAction("AttackCooldown")))
                              ->GetCooldownTime());
            agent->GetState().SetInAction(false);
            //agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            //    AnimationSystem::States::Idle);
            LOG_INFO("Finished Firing");
            return Success;
        }
        else
        {
            return OnGoing;
        }

	}

	void FlyingEnemyAttackAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
	{
        if (result == Failed)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

	std::unordered_map<EnemyGoal, float> FlyingEnemyAttackAction::InternalHeuristic(
	const EnemyState& state,
        const EnemyPreference& preference)
	{
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
	}

}  // namespace mts
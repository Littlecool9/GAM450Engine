/**
 * @file EnemyPatrolAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyPatrolAction.h"

#include <imgui_internal.h>

#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include <Engine/EntitySystem/Component/Transform.h>
#include <Engine/EntitySystem/GameObject/GameObject.h>
#include <Engine/EntitySystem/\Component/SpriteRenderer.h>
#include <fmt/base.h>

#include "../EnemyController.h"
#include "Engine/Helpers/Random.h"


namespace mts
{
    bool EnemyPatrolAction::IsAvailable(const EnemyState& state, const EnemyPreference& preferences) const
    {
        //stop patrolling if player in aggro range
        auto it = state.params.find(HashAction("AggroRange"));
        if (it != state.params.end() && it->second != nullptr)
        {
            auto* p = it->second;

            if (state.GetDistanceToPlayer() <= std::get<float>(p->GetValue()))
            {
                return false;
            }
        }
        return true; 
    }

    void EnemyPatrolAction::OnBeginning(EnemyAgent& agent)
    {

        //origin = agent.GetGameObject()->GetComponent<Transform>()->GetPosition();
        //facing = Random::Range(0, 10) < 5 ? 1 : -1;
        //agent.GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(facing < 0 ? false: true);
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Walking);
        //patrolRadius = agent.GetGameObject()->GetScript<EnemyController>()->GetPatrolRadius();
        //auto a = agent.GetGameObject()->GetScript<EnemyController>();
        //if (a && agent.GetGameObject()->GetScript<EnemyController>()->GetEnemyType() == EnemyType::FlyEnemy)
        //{
        //    flying = true;
        //}
        LOG_INFO("Patrolling!");
        //LOG_INFO("origin = {}, {}, {}", origin.x, origin.y, origin.z);
        //LOG_INFO("current pos = {}, {}, {}",
        //         agent.GetGameObject()->GetComponent<Transform>()->GetPosition().x,
        //         agent.GetGameObject()->GetComponent<Transform>()->GetPosition().y,
        //         agent.GetGameObject()->GetComponent<Transform>()->GetPosition().z);
        //LOG_INFO("patrolRadius = {}", patrolRadius);
        //LOG_INFO("facing = {}", facing);
    }

    float EnemyPatrolAction::GetEstimatedTimeCost(const EnemyState& state, const EnemyPreference& preferences) const
    {
        return 5.f;
    }

    EnemyState EnemyPatrolAction::GetExpectedNextState(const EnemyState& state, const EnemyPreference& preferences) const
    {
        EnemyState ns = state;
        return ns;
    }

    EnemyActionResult EnemyPatrolAction::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
            return Invalid;
        LOG_INFO("Update action ptr = {}", static_cast<void*>(this));
        auto s = agent->GetState();
        auto it = s.params.find(HashAction("AggroRange"));
        if (it != s.params.end() && it->second != nullptr)
        {
            auto* p = it->second;

            if (s.GetDistanceToPlayer() <= std::get<float>(p->GetValue()))
            {
                return Success;
            }
        }
        glm::vec3 origin = agent->GetGameObject()->GetScript<EnemyController>()->GetOriginPos();
        glm::vec3 pos = agent->GetGameObject()->GetComponent<Transform>()->GetPosition();

        float x = pos.x;
        
        int facing = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX() ? 1 : -1;

        // Move
        float spd = agent->GetGameObject()->GetScript<EnemyController>()->GetMoveSpeed(); 
        x += static_cast<float>(facing) * spd * dt;

        // Distance from origin
        float dist = x - origin.x;
        
        // Flip direction if beyond radius
        if (dist > agent->GetGameObject()->GetScript<EnemyController>()->GetPatrolRadius())
        {
            x = origin.x + agent->GetGameObject()
                               ->GetScript<EnemyController>()
                               ->GetPatrolRadius();  // clamp (prevents overshoot)
            facing = -1;
            agent->GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(false);
        }
        else if (dist < -agent->GetGameObject()->GetScript<EnemyController>()->GetPatrolRadius())
        {
            x = origin.x - agent->GetGameObject()->GetScript<EnemyController>()->GetPatrolRadius();
            facing = 1;
            agent->GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(true);
        }
        pos.x = x;
        if (agent->GetGameObject()->GetScript<EnemyController>()->GetEnemyType() == EnemyType::FlyEnemy)
        {
            pos.y = origin.y;
        }
        agent->GetGameObject()->GetComponent<Transform>()->SetPosition(pos);
        return OnGoing;
    }

    void EnemyPatrolAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
    }

    std::unordered_map<EnemyGoal, float> EnemyPatrolAction::InternalHeuristic(const EnemyState& state, const EnemyPreference& preference)
    {
        // Heuristic: unemployed behavior, only patrol when there's nothing to do, should be bottom of food chain
        return {{EnemyGoal::PlayerElimination, 0.1f},
                {EnemyGoal::SelfPreservation, 0.1f}};
    }
}  // namespace mts
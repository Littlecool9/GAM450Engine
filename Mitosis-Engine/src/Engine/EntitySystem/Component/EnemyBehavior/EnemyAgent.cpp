/**
 * @file EnemyAgent.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyAgent.h"

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    #define M_PI 3.14159265358979323846f

    EnemyAgent::EnemyAgent()
    {
    }

    // This is separate from Init() because we want to be able to set the planner reference before
    // the first Update() call, but after construction (since construction may be before the planner
    // instance is ready)
    void EnemyAgent::Initialize(AIPlanner& planner)
	{

		mPlanner = &planner;
        ResetExecution();
    }

    void EnemyAgent::Init()
    {
        ResetExecution();

    }

    void EnemyAgent::Update(float dt)
    {
        UpdateStateFromWorld(dt);
        bool hasPlanOrExecuting = PlanningAndPlanExecution(dt);
        // if want behavior for when agent isnt executing actions
        // if (!hasPlanOrExecuting)...
    }
    
    // builds a plan based on the current state and preference, but does not start executing it
    AIPlan EnemyAgent::BuildPlan()
    {
        AIPlan outPlan({}, 0.0f);
        mPlanner->BuildPlan(mState, mPreference, outPlan);
        return outPlan;
    }

    // helper to generate random numbers with a normal distribution
    float EnemyAgent::NormalDistRandom(float left, float right)
    {
        std::uniform_real_distribution<float> unit01(0.0f, 1.0f);
        const float f1 = std::max(unit01(mRng), 0.0001f);
        const float f2 = unit01(mRng);

        // Box-Muller-ish, then clamp like the Unity version
        float z = std::fabs(std::sqrt(-2.0f * std::log(f1)) * std::cos(2.0f * M_PI * f2));
        z = std::clamp(z / 2.0f, 0.0f, 1.0f);

        return left + (right - left) * z;
    }

    // helper to generate random numbers with an exponential distribution based on a normal
    // distribution, used for planning timer randomization
    float EnemyAgent::ExponentialNormalDistRandom(float x)
    {
        if (x < 0.0f)
            x = -x;
        if (x < 1.0f)
            x = 1.0f / x;

        const float ln = std::log(x);
        return std::exp(NormalDistRandom(-ln, ln));
    }

    // resets plan execution state, but does not clear the current plan (since it may be used for
    // learning updates after finishing/abandoning a plan)
    void EnemyAgent::ResetExecution()
    {
        mExecutingPlan = false;
        mStepIndex = 0;
        mStepTimer = 0.0f;
        mPlanningTimer = 0.0f;
        mlastPlanUtility = 0.0f;
        mCurrentPlan = AIPlan({}, 0.0f);
    }

    // starts executing a plan, resetting relevant timers and indices. 
    void EnemyAgent::StartPlan(AIPlan plan)
    {
        mCurrentPlan = std::move(plan);
        mExecutingPlan = !mCurrentPlan.Empty();
        mStepIndex = 0;
        mStepTimer = 0.0f;

        // Save state at plan start for adaptive updates
        mSavedStateForLearning = mState;

        // Call OnBeginning on the first action
        if (mExecutingPlan && !mCurrentPlan.GetSteps().empty())
        {
            BaseEnemyAction* action = mCurrentPlan.GetSteps()[0].GetAction();
            if (action)
            {
                // do action init if any
                action->OnBeginning(*this);
            }
        }
    }

    //run current task and returns whether the current step is still ongoing, has finished successfully, or has failed
    EnemyActionResult EnemyAgent::UpdateCurrentStep(float dt)
    {
        if (!mExecutingPlan)
            return Invalid;

        auto steps = mCurrentPlan.GetSteps();
        if (mStepIndex < 0 || mStepIndex >= static_cast<int>(steps.size()))
            return Success;

        BaseEnemyAction* action = steps[mStepIndex].GetAction();
        if (!action)
            return Failed;

        mStepTimer += dt;


        EnemyActionResult res = action->Update(this, dt, mStepTimer);


        if (res == OnGoing)
            return res;

        // OnFinished hook
        action->OnEnd(*this, res);

        if (res == Failed)
            return res;

        // SUCCESS: advance to next action
        ++mStepIndex;
        mStepTimer = 0.0f;

        if (mStepIndex >= static_cast<int>(steps.size()))
            return Success;

        // Begin next action
        BaseEnemyAction* next = steps[mStepIndex].GetAction();
        if (!next)
            return Failed;

        // revalidate with *current* state (after cooldown changes)
        if (!next->IsAvailable(GetState(), mPreference))
            return Failed;  // or trigger replanning instead of failing

        next->OnBeginning(*this);
        return OnGoing;
    }

    // returns whether we have a valid plan or are currently executing one (used for behavior when
    // not executing any plan)
    bool EnemyAgent::PlanningAndPlanExecution(float dt)
    {
        assert(mPlanner && "Error: Init() was not called, please provide planner id through init");

        mPlanningTimer -= dt;

        if (mPlanningTimer <= 0.0f && !mExecutingPlan)
        {
            mPlanningTimer += NormalDistRandom(0.2f, 1.0f);

            AIPlan newPlan = BuildPlan();

            if (!newPlan.Empty())
            {
                StartPlan(std::move(newPlan));
                return true;
            }

            //If we’re not executing and couldnt get a plan
            if (!mExecutingPlan)
                return false;
        }

        //Continue executing current plan
        EnemyActionResult res = UpdateCurrentStep(dt);

        if (res == Failed)
        {
            auto steps = mCurrentPlan.GetSteps();
            for (int i = 0; i <= mStepIndex && i < static_cast<int>(steps.size()); ++i)
            {
                BaseEnemyAction* action = steps[i].GetAction();
                if (action)
                {
                    action->AdaptiveHeuristicUpdate(mPreference, mSavedStateForLearning, mState);
                }
            }

            mlastPlanUtility = 0.0f;
            mExecutingPlan = false;
            mCurrentPlan = AIPlan({}, 0.0f);
            return true;
        }

        if (res == Success)
        {
            // Remaining cost estimate
            float cost = 0.0f;
            EnemyState simState = mState;

            auto steps = mCurrentPlan.GetSteps();
            for (int i = mStepIndex; i < (int) steps.size(); ++i)
            {
                BaseEnemyAction* a = steps[i].GetAction();
                if (!a)
                    continue;
                const float t = a->GetEstimatedTimeCost(simState, mPreference);
                cost += t;
                simState = a->GetExpectedNextState(simState, mPreference);
            }

            const float currScore = mState.GetScoreTowardsGoals(mPreference);

            if (!steps.empty())
            {
                BaseEnemyAction* last = steps.back().GetAction();
                if (last)
                {
                    const auto hMap = last->UtilityHeuristic(simState, mPreference);
                    const float h = mPreference.EvaluateHeuristic(hMap, cost);
                    mlastPlanUtility =
                        mPlanner->EvaluateNodeUtility(simState, mPreference, cost, currScore) + h;
                }
            }


            for (auto& s : steps)
            {
                s.GetAction()->AdaptiveHeuristicUpdate(mPreference, mSavedStateForLearning, mState);
            }

            mExecutingPlan = false;
            mCurrentPlan = AIPlan({}, 0.0f);
            return true;
        }

        return true;
    }

    void EnemyAgent::Deserialize(mts::JsonValue const& jsonObj)
    {
        if (jsonObj.Has("Plan Name"))
        {
            std::string s = jsonObj["Plan Name"].GetString();
            Initialize(AIPlanner::GetPlannerById(s));
        }
    }

    void EnemyAgent::Serialize(mts::JsonDocument& json) const
    {
        //TODO: Add
        json.WriteString("Type", "EnemyAgent");
        json.BeginObject("Data");

        json.WriteString("Plan Name", AIPlanner::GetPlannerId(*mPlanner).c_str());

        json.EndObject();  // end Data
    }

    // queries the world and updates state variables accordingly, should be called at the beginning
    // of each Update()
    void EnemyAgent::UpdateStateFromWorld(float dt)
    {

        // set state variables by querying the world here, e.g. distance to player, player health
        // ratio, etc
        //mstate.parameter....
        if (mPlayerRef)
        {
            mState.SetDistanceToPlayer(
                glm::distance(mPlayerRef->GetComponent<Transform>()->GetPosition(),
                              GetGameObject()->GetComponent<Transform>()->GetPosition()));
            mState.SetCurrentPlayerState(mPlayerRef->GetComponent<AnimationSystem>()->GetCurrentState());
        }

        if (mState.GetAttackCountdown() > 0)
        {
            mState.SetAttackCountdown(mState.GetAttackCountdown() - dt);  
            //LOG_INFO("Attack Cooldown: {}", mState.GetAttackCountdown());
        }
        if (mState.GetAttack2Countdown() > 0)
        {
            mState.SetAttack2Countdown(mState.GetAttack2Countdown() - dt);
            //LOG_INFO("Attack 2 Cooldown: {}", mState.GetAttack2Countdown());
        }
        if (mState.GetDodgeCountdown() > 0)
        {
            mState.SetDodgeCountdown(mState.GetDodgeCountdown() - dt);
            //LOG_INFO("Dodge Cooldown: {}", mState.GetDodgeCountdown());
        }
    }

    Component* EnemyAgent::Clone(GameObject* newOwner) const
    {
        EnemyAgent* c = new EnemyAgent(*this);
        c->InitFromClone(newOwner);
        return c;
    }

}  // namespace mts
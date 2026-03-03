/**
 * @file EnemyAgent.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <random>

#include "AIPlan.h"
#include "AIPlanner.h"
#include "EnemyState.h"
#include "Engine/EntitySystem/Component/Component.h"
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{

    class EnemyAgent: public Component
    {
       public:

        EnemyAgent();
        EnemyState& GetState() { return mState; }
        void SetState(const EnemyState& state) { mState = state; }
        const EnemyPreference& Preference() const { return mPreference; }
        void SetPreference(const EnemyPreference& preference) { mPreference = preference; }

        void Initialize(AIPlanner& planner);
        void Init() override;
        void Update(float dt) override;
        void SetPlayerReference(GameObject* player) { mPlayerRef = player; }
        GameObject* GetPlayerReference() const { return mPlayerRef; }
        void Deserialize(mts::JsonValue const& fields) override;
        void Serialize(mts::JsonDocument& out) const override;
        Component* Clone(GameObject* newOwner) const override;

       private:
        //private functions
        void UpdateStateFromWorld(float dt);
        AIPlan BuildPlan(); 
        bool PlanningAndPlanExecution(float dt);
        void StartPlan(AIPlan plan);
        EnemyActionResult UpdateCurrentStep(float dt);
        void ResetExecution();
        // Random helpers (ports of NormalDistRandom / ExponentialNormalDistRandom)
        float NormalDistRandom(float left, float right);
        float ExponentialNormalDistRandom(float x);
        //

        EnemyState mState;
        EnemyState mSavedStateForLearning{};
        GameObject* mPlayerRef = nullptr;  // reference to player object for state queries, can be
                                           // set in editor or found at runtime
        AIPlanner* mPlanner = nullptr;
        AIPlan mCurrentPlan{{}, 0.0f};
        EnemyPreference mPreference{};
        float mlastPlanUtility = 0.0f;
        int mStepIndex = 0;
        float mStepTimer = 0.0f;      // time spent in current action
        float mPlanningTimer = 0.0f;  // countdown until next replan attempt
        bool mExecutingPlan = false;
        std::mt19937 mRng{std::random_device{}()};
    };

}  // namespace mts
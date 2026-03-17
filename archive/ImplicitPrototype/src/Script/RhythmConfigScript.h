/**
 * @file RhythmConfigScript.h
 * @author Jackey Cen
 * @brief NativeScript that exposes rhythm system configuration (beat windows,
 *        boost multipliers, rhythm lock settings) to JSON for easy debugging.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */
#pragma once
#include "Engine/Scripting/ScriptBase.h"

enum class BeatRating
{
    Miss,    
    Good,    
    Perfect  
};

struct BeatJudgement
{
    BeatRating rating = BeatRating::Miss;
    float accuracy = 1.0f;
    float boostMultiplier = 1.0f;
};

/**
 *
 * JSON Example:
 * @code
 * {
 *     "Type": "NativeScript",
 *     "Data": {
 *         "Class": "RhythmConfigScript",
 *         "Fields": {
 *             "PerfectWindowStart": 0.92,
 *             "PerfectWindowEnd": 0.08,
 *             "GoodWindowStart": 0.85,
 *             "GoodWindowEnd": 0.15,
 *             "PerfectBoost": 1.5,
 *             "GoodBoost": 1.25,
 *             "MissBoost": 1.0,
 *             "RhythmLockEnabled": false,
 *         }
 *     }
 * }
 * @endcode
 */

class RhythmConfigScript : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    SCRIPT_CLASS(RhythmConfigScript)

    // Only one effective RhythmConfig
    // --- Access the Instance ---
    static RhythmConfigScript* GetInstance() { return sInstance; }

    BeatJudgement JudgeBeatTiming() const;
    bool IsOnBeat() const;
    bool IsPerfectBeat() const;
    float GetBeatPhase() const;

    float GetPerfectWindowStart() const { return mPerfectWindowStart; }
    float GetPerfectWindowEnd() const { return mPerfectWindowEnd; }
    float GetGoodWindowStart() const { return mGoodWindowStart; }
    float GetGoodWindowEnd() const { return mGoodWindowEnd; }
    float GetPerfectBoost() const { return mPerfectBoost; }
    float GetGoodBoost() const { return mGoodBoost; }
    float GetMissBoost() const { return mMissBoost; }
    bool IsRhythmLockEnabled() const { return mRhythmLockEnabled; }

   private:
    // --- Beat Window Config ---
    float mPerfectWindowStart = 0.90f;
    float mPerfectWindowEnd = 0.10f;

    float mGoodWindowStart = 0.80f;
    float mGoodWindowEnd = 0.20f;

    // --- Ability Boost Multiplier ---
    float mPerfectBoost = 1.5f;  
    float mGoodBoost = 1.25f;    
    float mMissBoost = 1.0f;     

    // --- Rhythm Locked ---
    bool mRhythmLockEnabled = false;

    bool IsPhaseInWindow(float phase, float windowStart, float windowEnd) const;

    static RhythmConfigScript* sInstance;
};

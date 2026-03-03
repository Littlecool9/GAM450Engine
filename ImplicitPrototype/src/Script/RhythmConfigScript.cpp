/**
 * @file RhythmConfigScript.cpp
 * @author Jackey Cen
 * @brief See RhythmConfigScript.h
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */
#include "pch.h"
#include "RhythmConfigScript.h"

#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/Serialization/JsonAbstraction.h"
#include <glm/common.hpp>

RhythmConfigScript* RhythmConfigScript::sInstance = nullptr;

void RhythmConfigScript::OnCreate()
{
    sInstance = this;
    
    LOG_INFO("[RhythmConfigScript] Initialized with config:");
    LOG_INFO("  Perfect Window: [{:.2f}, {:.2f}]", mPerfectWindowStart, mPerfectWindowEnd);
    LOG_INFO("  Good Window:    [{:.2f}, {:.2f}]", mGoodWindowStart, mGoodWindowEnd);
    LOG_INFO("  Boosts: Perfect={:.2f}x, Good={:.2f}x, Miss={:.2f}x",
             mPerfectBoost, mGoodBoost, mMissBoost);
    LOG_INFO("  Rhythm Lock: {}", mRhythmLockEnabled ? "Enabled" : "Disabled");
}

void RhythmConfigScript::OnUpdate(float /*dt*/)
{

}

void RhythmConfigScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("PerfectWindowStart"))
        mPerfectWindowStart = fields["PerfectWindowStart"].GetFloat(mPerfectWindowStart);
    if (fields.Has("PerfectWindowEnd"))
        mPerfectWindowEnd = fields["PerfectWindowEnd"].GetFloat(mPerfectWindowEnd);
    if (fields.Has("GoodWindowStart"))
        mGoodWindowStart = fields["GoodWindowStart"].GetFloat(mGoodWindowStart);
    if (fields.Has("GoodWindowEnd"))
        mGoodWindowEnd = fields["GoodWindowEnd"].GetFloat(mGoodWindowEnd);

    if (fields.Has("PerfectBoost"))
        mPerfectBoost = fields["PerfectBoost"].GetFloat(mPerfectBoost);
    if (fields.Has("GoodBoost"))
        mGoodBoost = fields["GoodBoost"].GetFloat(mGoodBoost);
    if (fields.Has("MissBoost"))
        mMissBoost = fields["MissBoost"].GetFloat(mMissBoost);

    if (fields.Has("RhythmLockEnabled"))
        mRhythmLockEnabled = fields["RhythmLockEnabled"].GetBool(mRhythmLockEnabled);
}

void RhythmConfigScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("PerfectWindowStart", mPerfectWindowStart);
    out.WriteFloat("PerfectWindowEnd", mPerfectWindowEnd);
    out.WriteFloat("GoodWindowStart", mGoodWindowStart);
    out.WriteFloat("GoodWindowEnd", mGoodWindowEnd);

    out.WriteFloat("PerfectBoost", mPerfectBoost);
    out.WriteFloat("GoodBoost", mGoodBoost);
    out.WriteFloat("MissBoost", mMissBoost);

    out.WriteBool("RhythmLockEnabled", mRhythmLockEnabled);
}

void RhythmConfigScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Perfect Window Start", .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mPerfectWindowStart}, .step = 0.01f});
    fields.push_back({.name = "Perfect Window End", .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mPerfectWindowEnd}, .step = 0.01f});
    fields.push_back({.name = "Good Window Start", .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mGoodWindowStart}, .step = 0.01f});
    fields.push_back({.name = "Good Window End", .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mGoodWindowEnd}, .step = 0.01f});

    fields.push_back({.name = "Perfect Boost", .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mPerfectBoost}, .step = 0.05f});
    fields.push_back({.name = "Good Boost", .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mGoodBoost}, .step = 0.05f});
    fields.push_back({.name = "Miss Boost", .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mMissBoost}, .step = 0.05f});

    fields.push_back({.name = "Rhythm Lock", .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mRhythmLockEnabled}});
}

bool RhythmConfigScript::IsPhaseInWindow(float phase, float windowStart, float windowEnd) const
{
    if (windowStart > windowEnd)
    {
        return phase >= windowStart || phase <= windowEnd;
    }
    return phase >= windowStart && phase <= windowEnd;
}

BeatJudgement RhythmConfigScript::JudgeBeatTiming() const
{
    BeatJudgement result;
    result.rating = BeatRating::Miss;
    result.accuracy = 1.0f;
    result.boostMultiplier = mMissBoost;

    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    if (!conductor.IsMusicPlaying())
    {
        return result;
    }

    float phase = conductor.GetBeatPhase();

    float distanceFromBeat = phase <= 0.5f ? phase : (1.0f - phase);

    if (IsPhaseInWindow(phase, mPerfectWindowStart, mPerfectWindowEnd))
    {
        result.rating = BeatRating::Perfect;
        float windowSize = (mPerfectWindowStart > mPerfectWindowEnd)
                               ? (1.0f - mPerfectWindowStart + mPerfectWindowEnd)
                               : (mPerfectWindowEnd - mPerfectWindowStart);
        result.accuracy = distanceFromBeat / (windowSize * 0.5f);
        result.accuracy = glm::clamp(result.accuracy, 0.0f, 1.0f);
        result.boostMultiplier = mPerfectBoost;
    }
    else if (IsPhaseInWindow(phase, mGoodWindowStart, mGoodWindowEnd))
    {
        result.rating = BeatRating::Good;
        float windowSize = (mGoodWindowStart > mGoodWindowEnd)
                               ? (1.0f - mGoodWindowStart + mGoodWindowEnd)
                               : (mGoodWindowEnd - mGoodWindowStart);
        result.accuracy = distanceFromBeat / (windowSize * 0.5f);
        result.accuracy = glm::clamp(result.accuracy, 0.0f, 1.0f);
        result.boostMultiplier = mGoodBoost;
    }
    else
    {
        result.rating = BeatRating::Miss;
        result.accuracy = 1.0f;
        result.boostMultiplier = mRhythmLockEnabled ? 0.0f : mMissBoost;
    }

    return result;
}

bool RhythmConfigScript::IsOnBeat() const
{
    return JudgeBeatTiming().rating != BeatRating::Miss;
}

bool RhythmConfigScript::IsPerfectBeat() const
{
    return JudgeBeatTiming().rating == BeatRating::Perfect;
}

float RhythmConfigScript::GetBeatPhase() const
{
    return mts::ConductorSystem::Get().GetBeatPhase();
}

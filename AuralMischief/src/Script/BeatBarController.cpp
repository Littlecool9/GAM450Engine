/**
 * @file BeatBarController.cpp
 * @author dream
 * @brief  Positions a beatline overlay and animates multiple converging beat markers
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BeatBarController.h"

#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/Component/UITransform.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"

void BeatBarController::Awake()
{
    mLeftMarkerTransforms .resize(mBarCount);
    mRightMarkerTransforms.resize(mBarCount);
    mLeftMarkerRenderers  .resize(mBarCount);
    mRightMarkerRenderers .resize(mBarCount);

    for (int i = 0; i < mBarCount; i++)
    {
        mts::GameObject* left  = mts::GameObjectFactory::CreateFromArchetype("Data/BeatMarker.prefab");
        mts::GameObject* right = mts::GameObjectFactory::CreateFromArchetype("Data/BeatMarker.prefab");

        mLeftMarkerTransforms[i]  = left ->GetComponent<mts::UITransform>();
        mRightMarkerTransforms[i] = right->GetComponent<mts::UITransform>();
        mLeftMarkerRenderers[i]   = left ->GetComponent<mts::SpriteRenderer>();
        mRightMarkerRenderers[i]  = right->GetComponent<mts::SpriteRenderer>();
    }
}

void BeatBarController::OnUpdate(float /*dt*/)
{
    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();

    // Global phase: 0..1 over mBarCount beats. One full cycle = mBarCount beats.
    const float globalPhase = ((float)(conductor.GetBeatIndex() % mBarCount) + conductor.GetBeatPhase())
                              / (float)mBarCount;

    // Fade ramp: alpha reaches 1.0 when this fraction of the phase range remains.
    const float fadeRampEnd = std::max(1e-4f, 1.0f - mAlphaFadeDistance);

    for (int i = 0; i < mBarCount; i++)
    {
        // barPhase 0 = marker at screen edge; barPhase 1 = marker at center.
        float barPhase   = std::fmod(globalPhase + (float)i / (float)mBarCount, 1.0f);
        // offsetNorm: 0.5 at edge, 0.0 at center (half-screen in normalized units)
        float offsetNorm = 0.5f * (1.0f - barPhase);
        float alpha      = std::min(barPhase / fadeRampEnd, 1.0f);

        // offsetNorm 0..0.5 (half-screen) → ref-px 0..800 from BottomCenter anchor
        float offsetPx = offsetNorm * 1600.0f;
        mLeftMarkerTransforms[i] ->SetOffset({-offsetPx, kBeatCircleOffsetY});
        mRightMarkerTransforms[i]->SetOffset({ offsetPx, kBeatCircleOffsetY});
        mLeftMarkerRenderers[i]  ->SetAlpha(alpha);
        mRightMarkerRenderers[i] ->SetAlpha(alpha);
    }
}

void BeatBarController::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("BarCount"))
        mBarCount = fields["BarCount"].GetInt(mBarCount);
    if (fields.Has("AlphaFadeDistance"))
        mAlphaFadeDistance = fields["AlphaFadeDistance"].GetFloat(mAlphaFadeDistance);
}

void BeatBarController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteInt  ("BarCount",          mBarCount);
    out.WriteFloat("AlphaFadeDistance", mAlphaFadeDistance);
}

void BeatBarController::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Bar Count",
                      .type = mts::ScriptField::Type::Int,
                      .ptr  = {.i = &mBarCount},
                      .step = 1.0f});
    fields.push_back({.name = "Alpha Fade Distance",
                      .type = mts::ScriptField::Type::Float,
                      .ptr  = {.f = &mAlphaFadeDistance},
                      .step = 0.01f});
}

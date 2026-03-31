/**
 * @file BeatPulseScript.cpp
 */
#include "pch.h"
#include "BeatPulseScript.h"

#include "Engine/EntitySystem/Component/UITransform.h"
#include "Engine/Serialization/JsonAbstraction.h"
#include "Engine/Renderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include <cmath>

#include "CameraController.h"

BeatPulseScript::~BeatPulseScript()
{
    if (mBeatSubId != 0)
        mts::ConductorSystem::Get().onBeat.Unsubscribe(mBeatSubId);
}

void BeatPulseScript::Init()
{
    mTransform = GetGameObject()->GetComponent<mts::UITransform>();
    if (mTransform)
        mBaseScale = mTransform->GetScale();
}

void BeatPulseScript::Awake()
{
    mBeatSubId =
        mts::ConductorSystem::Get().onBeat.Subscribe([this](mts::BeatInfo info) { OnBeat(info); });
}

void BeatPulseScript::OnDisable()
{
    if (mBeatSubId != 0)
    {
        mts::ConductorSystem::Get().onBeat.Unsubscribe(mBeatSubId);
        mBeatSubId = 0;
    }
    // Restore base scale immediately when disabled
    if (mTransform)
        mTransform->SetScale(mBaseScale);
    mAnimating = false;
}

void BeatPulseScript::OnBeat(mts::BeatInfo info)
{
    if (info.isHalfBeat)
        return;  // full beats only
    mTimer = 0.0f;
    mAnimating = true;
}

void BeatPulseScript::OnUpdate(float dt)
{
    if (!mAnimating || !mTransform)
        return;

    mTimer += dt;
    float residual = mAmplitude * std::exp(-mDecay * mTimer);

    if (residual < 0.001f)
    {
        mTransform->SetScale(mBaseScale);
        mAnimating = false;
        return;
    }

    float s = 1.0f + residual;
    mTransform->SetScale({mBaseScale.x * s, mBaseScale.y * s, mBaseScale.z});
}

void BeatPulseScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("Amplitude"))
        mAmplitude = fields["Amplitude"].GetFloat(mAmplitude);
    if (fields.Has("Decay"))
        mDecay = fields["Decay"].GetFloat(mDecay);
    if (fields.Has("XPos"))
        mXPos = fields["XPos"].GetFloat(mXPos);
    if (fields.Has("YPos"))
        mYPos = fields["YPos"].GetFloat(mYPos);
}

void BeatPulseScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("Amplitude", mAmplitude);
    out.WriteFloat("Decay", mDecay);
    out.WriteFloat("XPos", mXPos);
    out.WriteFloat("YPos", mYPos);
}

void BeatPulseScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Amplitude (A)",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAmplitude},
                      .step = 0.01f});
    fields.push_back({.name = "Decay (k)",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDecay},
                      .step = 0.5f});
    fields.push_back({.name = "XPos",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mXPos},
                      .step = 0.01f});
    fields.push_back({.name = "YPos",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mYPos},
                      .step = 0.01f});
}

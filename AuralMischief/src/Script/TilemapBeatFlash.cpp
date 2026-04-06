/**
 * @file TilemapBeatFlash.cpp
 * @author sumin.park
 * @brief  makes tilemap flash on given beat
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "TilemapBeatFlash.h"

void TilemapBeatFlash::Init()
{
}

void TilemapBeatFlash::Start()
{
    mTilemap = nullptr;
    mTilemap = GetGameObject()->GetComponent<mts::Tilemap::Tilemap>();
    if (!mTilemap)
        LOG_WARN("No Tilemap");

    mBeatDelegateId =
        mts::ConductorSystem::Get().onBeat.Subscribe([this](mts::BeatInfo info) { OnBeat(info); });
}

void TilemapBeatFlash::OnUpdate(float dt)
{
    if (!mTilemap || mHoldTint || mFlashTimer <= 0.0f)
        return;

    float t = mFlashTimer / mFlashDuration;  // 1 at flash onset → 0 at white
    mTilemap->SetTintColor(glm::mix(glm::vec3(1.0f), mFlashColor, t));
    mFlashTimer = std::max(0.0f, mFlashTimer - dt);
    if (mFlashTimer == 0.0f)
        mTilemap->SetTintColor({1.0f, 1.0f, 1.0f});
}

void TilemapBeatFlash::OnDisable()
{
    if (mBeatDelegateId)
    {
        mts::ConductorSystem::Get().onBeat.Unsubscribe(mBeatDelegateId);
        mBeatDelegateId = 0;
    }
    mTintActive = false;
    mFlashTimer = 0.f;
    if (mTilemap)
        mTilemap->SetTintColor({1.0f, 1.0f, 1.0f});
}

void TilemapBeatFlash::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("FlashColor"))
        mFlashColor = fields["FlashColor"].GetVec3(mFlashColor);
    if (fields.Has("FlashDuration"))
        mFlashDuration = fields["FlashDuration"].GetFloat(mFlashDuration);
    if (fields.Has("BeatInterval"))
        mBeatInterval = fields["BeatInterval"].GetInt(mBeatInterval);
    if (fields.Has("HoldTint"))
        mHoldTint = fields["HoldTint"].GetBool(mHoldTint);
}

void TilemapBeatFlash::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteVec3("FlashColor", mFlashColor);
    out.WriteFloat("FlashDuration", mFlashDuration);
    out.WriteInt("BeatInterval", mBeatInterval);
    out.WriteBool("HoldTint", mHoldTint);
}

void TilemapBeatFlash::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Flash Tint",
                      .type = mts::ScriptField::Type::Color,
                      .ptr = {.v3 = &mFlashColor}});

    fields.push_back({.name = "Flash Duration",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFlashDuration},
                      .step = 0.01f});

    fields.push_back({.name = "Beat Interval",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mBeatInterval}});

    fields.push_back({.name = "Hold Tint",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mHoldTint}});
}

void TilemapBeatFlash::OnBeat(mts::BeatInfo info)
{
    if (info.isHalfBeat)
        return;
    if (info.beatIndex % mBeatInterval != 0)
        return;

    if (mHoldTint)
    {
        mTintActive = !mTintActive;
        if (mTilemap)
            mTilemap->SetTintColor(mTintActive ? mFlashColor : glm::vec3(1.0f));
    }
    else
    {
        mFlashTimer = mFlashDuration;
    }
}
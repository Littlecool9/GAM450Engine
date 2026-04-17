/**
 * @file DamageNumberScript.cpp
 * @author sumin.park
 * @brief  Animates a single digit sprite in a configurable direction while fading out, then
 * disables itself
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "DamageNumberScript.h"

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include <algorithm>

void DamageNumberScript::Init()
{
    mTransform = GetGameObject()->GetComponent<mts::Transform>();
    mSprite = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    MTS_ASSERT(mTransform && "DamageNumberScript requires Transform");
    MTS_ASSERT(mSprite && "DamageNumberScript requires SpriteRenderer");
}

void DamageNumberScript::OnEnable()
{
    mElapsed = 0.f;
    if (mSprite)
        mSprite->SetAlpha(1.f);
    else
        LOG_WARN("[DamageNumberScript] OnEnable — mSprite is null on {}",
                 GetGameObject()->GetName());
}

void DamageNumberScript::OnUpdate(float dt)
{
    mElapsed += dt;

    glm::vec3 pos = mTransform->GetPosition();
    pos.x += mFloatDirX * mFloatSpeed * dt;
    pos.y += mFloatDirY * mFloatSpeed * dt;
    mTransform->SetPosition(pos);

    float alpha = 1.f - (mElapsed / mDuration);
    mSprite->SetAlpha(alpha);

    if (mElapsed >= mDuration)
        GetGameObject()->SetActive(false);
}

void DamageNumberScript::Activate(int digitVal, glm::vec3 pos)
{
    if (!mTransform || !mSprite)
    {
        LOG_WARN("[DamageNumberScript] Activate — null component on {}",
                 GetGameObject()->GetName());
        return;
    }
    mTransform->SetPosition(pos);
    mElapsed = 0.f;
    mSprite->SetAlpha(1.f);
    mSprite->SetFrame(16 + std::clamp(digitVal, 0, 9));
}

void DamageNumberScript::ApplyMotion(float speed, float dirX, float dirY)
{
    mFloatSpeed = speed;
    mFloatDirX = dirX;
    mFloatDirY = dirY;
}

void DamageNumberScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("floatSpeed"))
        mFloatSpeed = fields["floatSpeed"].GetFloat(mFloatSpeed);
    if (fields.Has("duration"))
        mDuration = fields["duration"].GetFloat(mDuration);
    if (fields.Has("floatDirX"))
        mFloatDirX = fields["floatDirX"].GetFloat(mFloatDirX);
    if (fields.Has("floatDirY"))
        mFloatDirY = fields["floatDirY"].GetFloat(mFloatDirY);
}

void DamageNumberScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("floatSpeed", mFloatSpeed);
    out.WriteFloat("duration", mDuration);
    out.WriteFloat("floatDirX", mFloatDirX);
    out.WriteFloat("floatDirY", mFloatDirY);
}

void DamageNumberScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Float Speed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFloatSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "Duration",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDuration},
                      .step = 0.05f});
    fields.push_back({.name = "Float Dir X",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFloatDirX},
                      .step = 0.1f});
    fields.push_back({.name = "Float Dir Y",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFloatDirY},
                      .step = 0.1f});
}

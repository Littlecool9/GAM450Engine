/**
 * @file AttackHitboxScript.cpp
 * @author josh.siegel
 * @brief // Script to control attack hitboxes spawned by player
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AttackHitboxScript.h"

void AttackHitboxScript::OnCreate()
{
    lifeTimer = mLifetime;
}

void AttackHitboxScript::OnEnable()
{
    lifeTimer = mLifetime;
}

void AttackHitboxScript::OnUpdate(float dt)
{
    lifeTimer -= dt;
    if (lifeTimer < 0.0f)
        GetGameObject()->SetActive(false);
}

void AttackHitboxScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("lifetime"))
        mLifetime = fields["lifetime"].GetFloat(mLifetime);
    if (fields.Has("damage"))
        mDamage = fields["damage"].GetFloat(mDamage);
}

void AttackHitboxScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("lifetime", mLifetime);
    out.WriteFloat("damage", mDamage);
}
/**
 * @file EndPointScript.cpp
 * @author sumin.park
 * @brief  trigger win
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EndPointScript.h"

#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

void EndPointScript::OnCollisionEnter2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->HasTag("Player"))
        mts::SceneSystem::SwitchScene(mTargetSceneIndex);
}

void EndPointScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("TargetSceneIndex"))
        mTargetSceneIndex = fields["TargetSceneIndex"].GetInt(mTargetSceneIndex);
}

void EndPointScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteInt("TargetSceneIndex", mTargetSceneIndex);
}

void EndPointScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "TargetSceneIndex",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mTargetSceneIndex},
                      .step = 1});
}

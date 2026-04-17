/**
 * @file CheckPoint.cpp
 * @author Jackey Cen
 * @brief: CheckPoint script for player respawn logic. When the player collides with this
 * checkpoint
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CheckPoint.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"

#include "Player/PlayerController.h"
#include "EnemyBehaviors/EnemyController.h"
#include "BossRoomTrigger.h"

void CheckPoint::Init()
{
    mBossGO = nullptr;
    mBossEC = nullptr;
    mBossRoomTriggerGO = nullptr;
    mBossRoomTrigger = nullptr;
    mSr = nullptr;
}

void CheckPoint::Start()
{
    mSr = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    mCollider = GetGameObject()->GetComponent<mts::Collider2D>();

    if (mIsBossLocked && mBossUUID.IsValid())
    {
        auto* scene = mts::SceneSystem::GetCurrentScene();
        if (scene)
        {
            mBossGO = scene->FindGameObjectByUUID(mBossUUID);
            if (mBossGO)
                mBossEC = mBossGO->GetScript<mts::EnemyController>();

            if (mBossRoomTriggerUUID.IsValid())
            {
                mBossRoomTriggerGO = scene->FindGameObjectByUUID(mBossRoomTriggerUUID);
                if (mBossRoomTriggerGO)
                    mBossRoomTrigger = mBossRoomTriggerGO->GetScript<BossRoomTrigger>();
            }
        }
    }
}

void CheckPoint::OnUpdate(float dt)
{
    if (!mIsBossLocked || !mSr)
        return;

    auto* scene = mts::SceneSystem::GetCurrentScene();

    // Validate cached boss GO still in scene — clears cache when boss is destroyed
    if (mBossEC)
    {
        if (!scene || !scene->FindGameObjectByUUID(mBossUUID))
        {
            mBossEC = nullptr;
            mBossGO = nullptr;
        }
    }

    // Validate cached room trigger GO still in scene
    if (mBossRoomTrigger)
    {
        if (!scene || !scene->FindGameObjectByUUID(mBossRoomTriggerUUID))
        {
            mBossRoomTrigger = nullptr;
            mBossRoomTriggerGO = nullptr;
        }
    }

    mSr->SetTintColor(IsBossLocking() ? mLockedTint : glm::vec3(1.0f, 1.0f, 1.0f));
    mCollider->SetIsTrigger(!IsBossLocking());
}

void CheckPoint::OnDisable()
{
    if (mSr)
        mSr->SetTintColor({1.0f, 1.0f, 1.0f});
}

void CheckPoint::OnCollisionEnter2D(mts::Collision2D const& collision)
{
    if (mIsBossLocked && IsBossLocking())
        return;

    if (collision.other->GetGameObject()->GetName() == "Player")
    {
        PlayerController* playerScript =
            collision.other->GetGameObject()->GetScript<PlayerController>();
        if (!playerScript)
        {
            LOG_WARN("[CheckPoint] Collided with Player but no PlayerController script found");
            return;
        }
        playerScript->SetCheckPoint(this);
        playerScript->Heal(
            playerScript->GetMaxHP());  // Heal player to full on checkpoint activation
    }
}

void CheckPoint::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("IsBossLocked"))
        mIsBossLocked = fields["IsBossLocked"].GetBool(mIsBossLocked);
    if (fields.Has("BossUUID"))
        mBossUUID = mts::UUID::FromString(fields["BossUUID"].GetString());
    if (fields.Has("LockedTint"))
        mLockedTint = fields["LockedTint"].GetVec3(mLockedTint);
    if (fields.Has("BossRoomTriggerUUID"))
        mBossRoomTriggerUUID = mts::UUID::FromString(fields["BossRoomTriggerUUID"].GetString());
}

void CheckPoint::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteBool("IsBossLocked", mIsBossLocked);
    if (mBossUUID.IsValid())
        out.WriteString("BossUUID", mBossUUID.ToString().c_str());
    out.WriteVec3("LockedTint", mLockedTint);
    if (mBossRoomTriggerUUID.IsValid())
        out.WriteString("BossRoomTriggerUUID", mBossRoomTriggerUUID.ToString().c_str());
}

void CheckPoint::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Is Boss Locked",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mIsBossLocked}});
    fields.push_back({.name = "Boss GO",
                      .type = mts::ScriptField::Type::GameObject,
                      .ptr = {.go = &mBossGO},
                      .uuid = &mBossUUID});
    fields.push_back({.name = "Locked Tint",
                      .type = mts::ScriptField::Type::Color,
                      .ptr = {.v3 = &mLockedTint}});
    fields.push_back({.name = "Boss Room Trigger GO",
                      .type = mts::ScriptField::Type::GameObject,
                      .ptr = {.go = &mBossRoomTriggerGO},
                      .uuid = &mBossRoomTriggerUUID});
}

bool CheckPoint::IsBossLocking()
{
    if (!mBossEC)
        return false;

    bool isBossAlive = mBossEC->GetCurHP() > 0;
    bool isPlayerInRoom = !mBossRoomTrigger || mBossRoomTrigger->IsPlayerInside();

    if (!isPlayerInRoom)
        return false;
    else
        return isBossAlive;
}

glm::vec3 CheckPoint::GetCheckPointPosition() const
{
    mts::Transform* transform = GetGameObject()->GetComponent<mts::Transform>();
    if (!transform)
    {
        LOG_ERROR("[CheckPoint] No Transform component found on CheckPoint");
        return glm::vec3(0.0f);
    }
    return transform->GetPosition();
}

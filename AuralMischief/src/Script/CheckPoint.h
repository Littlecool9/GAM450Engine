/**
 * @file CheckPoint.h
 * @author Jackey Cen
 * @brief: CheckPoint script for player respawn logic. When the player collides with this
 * checkpoint,
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/UUID/UUID.h"

class BossRoomTrigger;

namespace mts
{
    class Collider2D;
    class Transform;
    class GameObject;
    class SpriteRenderer;
    class EnemyController;
}  // namespace mts

class CheckPoint : public mts::ScriptBase
{
   public:
    void Init() override;
    void Start() override;
    void OnUpdate(float dt) override;
    void OnDisable() override;
    void OnCollisionEnter2D(mts::Collision2D const& collision) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    glm::vec3 GetCheckPointPosition() const;

    SCRIPT_CLASS(CheckPoint)

   private:
    bool IsBossLocking();

   private:
    bool mIsBossLocked = false;
    mts::UUID mBossUUID = mts::UUID::InvalidID;
    mts::GameObject* mBossGO = nullptr;
    mts::EnemyController* mBossEC = nullptr;
    mts::UUID mBossRoomTriggerUUID = mts::UUID::InvalidID;
    mts::GameObject* mBossRoomTriggerGO = nullptr;
    BossRoomTrigger* mBossRoomTrigger = nullptr;
    glm::vec3 mLockedTint = {1.0f, 0.3f, 0.3f};
    mts::SpriteRenderer* mSr = nullptr;
    mts::Collider2D* mCollider = nullptr;
};

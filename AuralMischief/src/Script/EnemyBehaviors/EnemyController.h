/**
 * @file EnemyController.h
 * @author sumin.park
 * @brief  Enemy behavior script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Audio/AudioTypes.h"

class PlayerController;
class EnemyAudioController;

namespace mts
{
    enum class EnemyType
    {
        CrawlEnemy,
        FlyEnemy,
        SentryEnemy,
        BossEnemy

    };

    class EnemyController : public ScriptBase
    {
       public:
        void Init() override;
        void Start() override;
        void OnUpdate(float dt) override;
        void OnFixedUpdate(float) override;
        void DeserializeFields(JsonValue const& fields) override;
        void SerializeFields(JsonDocument& out) const override;
        void OnCollisionEnter2D(const Collision2D& collision) override;
        int GetCurHP() { return curhp; }
        void SetEnemyPlans(std::string name);
        glm::vec3 GetOriginPos() { return origin; }
        EnemyType GetEnemyType() { return enemyType; }
        float GetMoveSpeed() { return mMoveSpeed; }
        float GetPatrolRadius() { return mPatrolRadius; }
        void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;
        void MarkDead() { curhp = 0; }
        void TakeDamage(int dmg);
        bool IsDying() const { return mDying; }
        bool IsExploding() const { return mShouldExplode; }
        void Explode();
        float GetAggroRange() { return mAggroRange; }
        float GetAttackRange() { return mAttackRange; }
        float GetAggroYRange() { return mAggroYRange; }
        bool IsPlayerInYRange(const glm::vec2& enemyPos, const glm::vec2& playerPos) const
        {
            return std::abs(playerPos.y - enemyPos.y) <= mAggroYRange;
        }
        void SetDesiredVelocity(glm::vec2 vel) { mDesiredVelocity = vel; }

        // Attack hitbox management — per-attack storage
        // Attack1
        void SetAtk1Hitbox(GameObject* hb) { mAtk1Hitbox = hb; }
        GameObject* GetAtk1Hitbox() const { return mAtk1Hitbox; }
        // Attack2
        void SetAtk2Hitbox(GameObject* hb) { mAtk2Hitbox = hb; }
        GameObject* GetAtk2Hitbox() const { return mAtk2Hitbox; }
        // Attack3
        void SetAtk3Hitbox(GameObject* hb) { mAtk3Hitbox = hb; }
        GameObject* GetAtk3Hitbox() const { return mAtk3Hitbox; }
        void SetAtk3HitboxFront2(GameObject* hb) { mAtk3HitboxFront2 = hb; }
        GameObject* GetAtk3HitboxFront2() const { return mAtk3HitboxFront2; }
        void SetAtk3HitboxBehind(GameObject* hb) { mAtk3HitboxBehind = hb; }
        GameObject* GetAtk3HitboxBehind() const { return mAtk3HitboxBehind; }

        // Per-enemy conductor subscription IDs for each attack action
        void SetConductorSubId(const std::string& actionName, uint32_t id)
        {
            mConductorSubIds[actionName] = id;
        }
        uint32_t GetConductorSubId(const std::string& actionName) const
        {
            auto it = mConductorSubIds.find(actionName);
            return (it != mConductorSubIds.end()) ? it->second : 0;
        }
        void ClearConductorSubId(const std::string& actionName)
        {
            mConductorSubIds.erase(actionName);
        }

        void PlayBossMoanSFX();

        SCRIPT_CLASS(EnemyController)

       private:
        EnemyType enemyType = EnemyType::CrawlEnemy;
        glm::vec3 origin;
        std::string type;
        bool mDying = false;
        bool mShouldExplode = false;
        bool mFadingOut = false;
        bool mAttack2Done = false;
        float mMoveSpeed = 1.5f;
        float mPatrolRadius = 2.f;
        float mAggroRange = 4.f;
        float mAttackRange = 1.f;
        float mAggroYRange = 8.5f;
        float mAttackCooldown = 5.f;
        float mAttack2Cooldown = 5.f;
        float mAttack3Cooldown = 5.f;
        float mDodgeCooldown = 5.f;
        int maxhp = 2;
        int curhp = 0;
        bool alerted = false;
        bool mIsInvincible = false;
        bool deathAnimFirstTime = true;
        bool mDeathBeepPlayed = false;
        mts::AudioHandle mFuseHandle{};
        mts::AudioHandle mBossSFX{};
        float mExplosionRadiusMultiplier = 5;
        float mTimeTillExplode = 1;
        PlayerController* mPlayerController = nullptr;
        EnemyAudioController* mAudioController = nullptr;
        glm::vec2 mDesiredVelocity{0.0f, 0.0f};
        GameObject* mAtk1Hitbox = nullptr;
        GameObject* mAtk2Hitbox = nullptr;
        GameObject* mAtk3Hitbox = nullptr;
        GameObject* mAtk3HitboxFront2 = nullptr;
        GameObject* mAtk3HitboxBehind = nullptr;
        std::unordered_map<std::string, uint32_t> mConductorSubIds;
    };

}  // namespace mts
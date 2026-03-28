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

namespace mts
{
    enum class EnemyType
    {
        CrawlEnemy,
        FlyEnemy,
        SentryEnemy
        
    };

    class EnemyController : public ScriptBase
    {
       public:
        void OnCreate() override;
        void OnUpdate(float dt) override;
        void DeserializeFields(JsonValue const& fields) override;
        void SerializeFields(JsonDocument& out) const override;
        void OnCollisionEnter2D(const Collision2D& collision) override;
        int GetCurHP() { return curhp; }
        void SetEnemyPlans(std::string name);
        glm::vec3 GetOriginPos() { return origin; }
        EnemyType GetEnemyType() { return enemyType; }
        float GetMoveSpeed() {return mMoveSpeed;}
        float GetPatrolRadius() { return mPatrolRadius; }
        void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;
        SCRIPT_CLASS(EnemyController)

       private:
        EnemyType enemyType;
        glm::vec3 origin;
        std::string type;
        float mMoveSpeed = 1.5f;
        float mPatrolRadius = 2.f;
        float mAggroRange = 5.f;
        float mAttackCooldown = 5.f;
        int maxhp = 2;
        int curhp = 0;
        bool alerted = false;
    };

}

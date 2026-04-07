/**
 * @file BulletBehavior.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    enum class BulletOwner
    {
        Enemy,
        Player
    };

    class BulletBehavior : public ScriptBase
    {
       public:
        void Start() override;
        void OnUpdate(float dt) override;
        void DeserializeFields(JsonValue const& fields) override;
        void SerializeFields(JsonDocument& out) const override;
        void OnCollisionEnter2D(const Collision2D& collision) override;
        void SetDirection(glm::vec2 dir) { mDirection = dir; }
        void SetOwner(BulletOwner owner) { mOwner = owner; }
        BulletOwner GetOwner() const { return mOwner; }
        void SetHoming(bool homing) { mHoming = homing; }
        bool GetIsDying() const { return mDying; }

        SCRIPT_CLASS(BulletBehavior)

       private:
        void BeginDeath();

        float mBulletSpeed = 1.5f;
        float dmg = 2;
        float lifetime = 5.f;
        glm::vec2 mDirection{1.0f, 0.0f};
        Transform* playerTransform = nullptr;
        bool mDestroy = false;
        bool mDying = false;
        bool mHoming = false;
        BulletOwner mOwner = BulletOwner::Enemy;
    };

}  // namespace mts
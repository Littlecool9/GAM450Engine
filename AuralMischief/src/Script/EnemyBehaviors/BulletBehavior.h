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
    class BulletBehavior: public ScriptBase
    {
    public:
        void OnCreate() override;
        void OnUpdate(float dt) override;
        void DeserializeFields(JsonValue const& fields) override;
        void SerializeFields(JsonDocument& out) const override;
        void OnCollisionEnter2D(const Collision2D& collision) override;
        void SetDirection(glm::vec2 dir) { mDirection = dir; }

        SCRIPT_CLASS(BulletBehavior)


    private:
        float mBulletSpeed = 1.5f;
     float dmg = 1;
        float lifetime = 5.f;
     glm::vec2 mDirection{1.0f, 0.0f};
     Transform* playerTransform = nullptr;
     bool mDestroy = false;
    };

}  // namespace mts
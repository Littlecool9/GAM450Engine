/**
 * @file Collider2D.h
 * @author sumin.park
 * @brief  Base 2D Collider component.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "../Component.h"

namespace mts
{
    class Collider2D : public Component
    {
       public:
        enum class ColliderType : unsigned char
        {
            Circle,
            Box
        };

       public:
        virtual ~Collider2D() override;

        void Serialize(JsonDocument& doc) const override;
        void Deserialize(const JsonValue& jsonObj) override;

        virtual ColliderType GetColliderType() const = 0;
        virtual bool IsColliding(const Collider2D& collider) = 0;

        bool IsTrigger() const { return mIsTrigger; }

       private:
        bool mIsTrigger = false;
    };

}  // namespace mts
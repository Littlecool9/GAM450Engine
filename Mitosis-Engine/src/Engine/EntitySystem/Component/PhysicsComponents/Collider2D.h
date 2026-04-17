/**
 * @file Collider2D.h
 * @author sumin.park
 * @brief  Base 2D Collider component.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
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
            Box,
            None
        };

       public:
        virtual ~Collider2D() override;

        virtual ColliderType GetColliderType() const = 0;

        bool IsTrigger() const { return mIsTrigger; }

        void SetIsTrigger(bool isTrigger) { mIsTrigger = isTrigger; }

        // test purposes
        void SetIsColliding(bool isColliding) { mIsColliding = isColliding; }

       protected:
        bool IsColliding() { return mIsColliding; }

       private:
        bool mIsTrigger = false;
        bool mIsColliding = false;  // test purposes
    };

}  // namespace mts
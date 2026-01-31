/**
 * @file BoxCollider2D.cpp
 * @author sumin.park
 * @brief  BoxCollider2D Component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BoxCollider2D.h"

namespace mts
{
    BoxCollider2D::~BoxCollider2D()
    {
    }

    bool BoxCollider2D::IsColliding(const Collider2D& collider)
    {
        return false;
    }
}  // namespace mts
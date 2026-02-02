/**
 * @file CircleCollider2D.cpp
 * @author sumin.park
 * @brief  CircleCollider2D Component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CircleCollider2D.h"
#include "Engine/Debug/DebugDraw.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    CircleCollider2D::~CircleCollider2D()
    {
    }

    void CircleCollider2D::DebugRender()
    {
        Transform* transformtr = GetGameObject()->GetComponent<Transform>();
        glm::vec2 center = math::ToVec2(transformtr->GetPosition()) + mOffset;
        float radius = transformtr->GetScale().x * mRadius * 0.5f;
        glm::vec4 color = {0, 1.0, 0, 1.0};

        debug::DebugDraw::Circle(center, radius, 16, color);
    }

    bool CircleCollider2D::IsColliding(const Collider2D& collider)
    {
        return false;
    }

    void CircleCollider2D::Deserialize(const JsonValue& jsonObj)
    {
        if (jsonObj.Has("Radius"))
            mRadius = jsonObj["Radius"].GetFloat();
        if (jsonObj.Has("Offset"))
            mOffset = jsonObj["Offset"].GetVec2();
    }

    void CircleCollider2D::Serialize(JsonDocument& doc) const
    {
    }
}  // namespace mts
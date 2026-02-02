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

#include "Engine/Debug/DebugDraw.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    BoxCollider2D::~BoxCollider2D()
    {
    }

    void BoxCollider2D::DebugRender()
    {
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        glm::vec2 scale = math::ToVec2(tr->GetScale());
        glm::vec2 halfScale = scale * 0.5f;

        // this one works for the current sprite system will need sprite to change.
        // glm::vec2 center = math::ToVec2(tr->GetPosition()) + mOffset + halfScale;
        glm::vec2 center = math::ToVec2(tr->GetPosition()) + mOffset;

        glm::vec2 halfExtents = halfScale * glm::vec2{mWidth, mHeight};

        float rotation = tr->GetRotation();
        glm::vec4 color = {0, 1.0, 0, 1.0};

        debug::DebugDraw::Box(center, halfExtents, rotation, color);
    }

    bool BoxCollider2D::IsColliding(const Collider2D& collider)
    {
        return false;
    }

    void BoxCollider2D::Deserialize(const JsonValue& jsonObj)
    {
        if (jsonObj.Has("Width"))
            mWidth = jsonObj["Width"].GetFloat();

        if (jsonObj.Has("Height"))
            mHeight = jsonObj["Height"].GetFloat();

        if (jsonObj.Has("Offset"))
            mOffset = jsonObj["Offset"].GetVec2();
    }

    void BoxCollider2D::Serialize(JsonDocument& doc) const
    {
    }
}  // namespace mts
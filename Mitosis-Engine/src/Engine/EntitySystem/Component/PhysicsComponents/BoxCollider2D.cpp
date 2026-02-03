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

        // this one works for the current sprite system will need sprite to change.
        // glm::vec2 center = math::ToVec2(tr->GetPosition()) + mOffset + halfScale;
        glm::vec2 center = math::ToVec2(tr->GetPosition()) + mOffset;

        glm::vec2 halfExtents = scale * glm::vec2{mHalfWidth, mHalfHeight};

        float rotation = tr->GetRotation();
        glm::vec4 color = {0, 1.0, 0, 1.0};
        if (IsColliding())
            color = {1.0, 0.0, 0.0, 1.0};

        debug::DebugDraw::Box(center, halfExtents, rotation, color);
    }

    void BoxCollider2D::Deserialize(const JsonValue& jsonObj)
    {
        if (jsonObj.Has("Width"))
            SetWidth(jsonObj["Width"].GetFloat());
        if (jsonObj.Has("Height"))
            SetHeight(jsonObj["Height"].GetFloat());
        if (jsonObj.Has("Offset"))
            mOffset = jsonObj["Offset"].GetVec2();
        if (jsonObj.Has("IsTrigger"))
            SetIsTrigger(jsonObj["IsTrigger"].GetInt());
    }

    void BoxCollider2D::Serialize(JsonDocument& doc) const
    {
    }

    void BoxCollider2D::SetWidth(float width)
    {
        mWidth = width;
        mHalfWidth = width * 0.5f;
    }

    void BoxCollider2D::SetHeight(float height)
    {
        mHeight = height;
        mHalfHeight = height * 0.5f;
    }

    glm::vec2 BoxCollider2D::GetCenter() const
    {
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        return math::ToVec2(tr->GetPosition()) + mOffset;
    }

    glm::vec2 BoxCollider2D::GetHalfExtents() const
    {
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        return {tr->GetScale().x * mHalfWidth, tr->GetScale().y * mHalfHeight};
    }

    glm::vec2 BoxCollider2D::GetSize() const
    {
        return {mWidth, mHeight};
    }

    glm::vec2 BoxCollider2D::GetUp() const
    {
        // if any extra calc is done, remember to normalize
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        return tr->GetUp();
    }

    glm::vec2 BoxCollider2D::GetRight() const
    {
        // if any extra calc is done, remember to normalize
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        return tr->GetRight();
    }
}  // namespace mts
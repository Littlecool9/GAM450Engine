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
#include "Engine/Core/Editor/DebugDraw.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include <glm/vec4.hpp>

namespace mts
{
    BoxCollider2D::~BoxCollider2D()
    {
    }

    void BoxCollider2D::DebugRender()
    {
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        glm::vec2 scale = math::ToVec2(tr->GetScale());

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
            SetLocalWidth(jsonObj["Width"].GetFloat());
        if (jsonObj.Has("Height"))
            SetLocalHeight(jsonObj["Height"].GetFloat());
        if (jsonObj.Has("Offset"))
            mOffset = jsonObj["Offset"].GetVec2();
        if (jsonObj.Has("IsTrigger"))
            SetIsTrigger(jsonObj["IsTrigger"].GetInt());
    }

    void BoxCollider2D::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "BoxCollider2D");
        doc.BeginObject("Data");

        doc.WriteFloat("Width", mWidth);
        doc.WriteFloat("Height", mHeight);
        doc.WriteVec2("Offset", mOffset);

        doc.WriteInt("IsTrigger", IsTrigger() ? 1 : 0);

        doc.EndObject();
    }

    void BoxCollider2D::SetLocalWidth(float width)
    {
        mWidth = width;
        mHalfWidth = width * 0.5f;
    }

    void BoxCollider2D::SetLocalHeight(float height)
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

    glm::vec2 BoxCollider2D::GetLocalSize() const
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

    Component* BoxCollider2D::Clone(GameObject* newOwner) const
    {
        BoxCollider2D* c = new BoxCollider2D(*this);
        c->InitFromClone(newOwner);
        return c;
    }
}  // namespace mts
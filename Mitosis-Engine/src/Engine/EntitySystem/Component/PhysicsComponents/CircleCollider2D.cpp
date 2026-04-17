/**
 * @file CircleCollider2D.cpp
 * @author sumin.park
 * @brief  CircleCollider2D Component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "CircleCollider2D.h"
#include "Engine/Core/Editor/DebugDraw.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    CircleCollider2D::~CircleCollider2D()
    {
    }

    void CircleCollider2D::DebugRender()
    {
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        glm::vec2 center = math::ToVec2(tr->GetPosition()) + mOffset;
        float radius = tr->GetScale().x * mRadius * 0.5f;
        glm::vec4 color = {0, 1.0, 0, 1.0};
        if (IsColliding())
            color = {1.0, 0.0, 0.0, 1.0};

        debug::DebugDraw::Circle(center, radius, 16, color);
    }

    void CircleCollider2D::Deserialize(const JsonValue& jsonObj)
    {
        if (jsonObj.Has("Radius"))
            mRadius = jsonObj["Radius"].GetFloat();
        if (jsonObj.Has("Offset"))
            mOffset = jsonObj["Offset"].GetVec2();
        if (jsonObj.Has("IsTrigger"))
            SetIsTrigger(jsonObj["IsTrigger"].GetInt());
    }

    void CircleCollider2D::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "CircleCollider2D");
        doc.BeginObject("Data");

        doc.WriteFloat("Radius", mRadius);

        doc.WriteVec2("Offset", mOffset);

        doc.WriteInt("IsTrigger", IsTrigger() ? 1 : 0);

        doc.EndObject();
    }

    glm::vec2 CircleCollider2D::GetCenter() const
    {
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        return math::ToVec2(tr->GetPosition()) + mOffset;
    }

    float CircleCollider2D::GetRadius() const
    {
        Transform* tr = GetGameObject()->GetComponent<Transform>();
        return tr->GetScale().x * mRadius;
    }


    Component* CircleCollider2D::Clone(GameObject* newOwner) const
    {
        CircleCollider2D* c = new CircleCollider2D(*this);
        c->InitFromClone(newOwner);
        return c;
    }
}  // namespace mts
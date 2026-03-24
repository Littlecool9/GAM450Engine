/**
 * @file UITransform.cpp
 * @author sumin.park
 * @brief  Transform component for screen-space (HUD/UI) GameObjects.
 *         Owns position/scale/rotation in pixel coordinates; independent of Transform.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "UITransform.h"

#include <glm/ext/matrix_transform.hpp>

namespace mts
{
    const glm::vec3& UITransform::GetPosition() const { return mPosition; }

    void UITransform::SetPosition(const glm::vec3& pos)
    {
        mPosition = pos;
        mIsDirty  = true;
    }

    void UITransform::SetPosition(const glm::vec2& pos)
    {
        mPosition.x = pos.x;
        mPosition.y = pos.y;
        mIsDirty    = true;
    }

    const glm::vec3& UITransform::GetScale() const { return mScale; }

    void UITransform::SetScale(const glm::vec3& s)
    {
        mScale   = s;
        mIsDirty = true;
    }

    float UITransform::GetRotation() const { return mRotation; }

    void UITransform::SetRotation(float r)
    {
        mRotation = r;
        mIsDirty  = true;
    }

    glm::mat4 UITransform::GetLocalMatrix()
    {
        if (!mIsDirty)
            return mLocalMatrix;
        glm::mat4 S = glm::scale(glm::mat4(1.0f), mScale);
        glm::mat4 R = glm::rotate(glm::mat4(1.0f), mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 T = glm::translate(glm::mat4(1.0f), mPosition);
        mLocalMatrix = T * R * S;
        mIsDirty     = false;
        return mLocalMatrix;
    }

    void UITransform::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("Position"))
            mPosition = jsonObj["Position"].GetVec3();

        if (jsonObj.Has("Scale"))
            mScale = jsonObj["Scale"].GetVec3();

        if (jsonObj.Has("Rotation"))
            mRotation = jsonObj["Rotation"].GetFloat(mRotation);

        mIsDirty = true;
    }

    void UITransform::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "UITransform");
        doc.BeginObject("Data");

        doc.BeginArray("Position");
        doc.WriteFloat(mPosition.x);
        doc.WriteFloat(mPosition.y);
        doc.WriteFloat(mPosition.z);
        doc.EndArray();

        doc.BeginArray("Scale");
        doc.WriteFloat(mScale.x);
        doc.WriteFloat(mScale.y);
        doc.WriteFloat(mScale.z);
        doc.EndArray();

        doc.WriteFloat("Rotation", mRotation);

        doc.EndObject();
    }

    Component* UITransform::Clone(GameObject* newOwner) const
    {
        auto* c = new UITransform(*this);
        c->InitFromClone(newOwner);
        return c;
    }
}  // namespace mts

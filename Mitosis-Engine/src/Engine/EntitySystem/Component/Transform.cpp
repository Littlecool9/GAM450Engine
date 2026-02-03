/**
 * @file Transform.cpp
 * @author sumin.park
 * @brief  Transform Component
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Transform.h"

#include "Engine/Debug/DebugDraw.h"

namespace mts
{
    void Transform::DebugRender()
    {
        glm::vec2 pos = math::ToVec2(mPosition);
        glm::vec2 up = math::ToVec2(GetUp()) * 200.0f;
        up += pos;
        glm::vec2 right = math::ToVec2(GetRight()) * 200.0f;
        right += pos;
        debug::DebugDraw::Line(pos, up, {1.0, 0, 0, 1.0});
        debug::DebugDraw::Line(pos, right, {0.0, 0, 1.0, 1.0});
    }

    void Transform::SetPosition(const glm::vec3& pos)
    {
        mPosition = pos;
        mIsDirty = true;
    }

    void Transform::SetPosition(const glm::vec2& pos)
    {
        mPosition = math::ToVec3(pos, mPosition.z);
        mIsDirty = true;
    }

    const glm::vec3& Transform::GetPosition() const
    {
        return mPosition;
    }
    void Transform::SetScale(const glm::vec3& s)
    {
        mScale = s;
        mIsDirty = true;
    }
    const glm::vec3& Transform::GetScale() const
    {
        return mScale;
    }
    void Transform::SetRotation(float r)
    {
        mRotation = r;
        mIsDirty = true;
    }
    float Transform::GetRotation() const
    {
        return mRotation;
    }

    void Transform::Deserialize(JsonValue const& jsonObj)
    {
        glm::vec3 p(0);
        glm::vec3 s(0);
        float r(0);

        if (jsonObj.Has("Position"))
        {
            p = jsonObj["Position"].GetVec3();
        }

        if (jsonObj.Has("Scale"))
        {
            s = jsonObj["Scale"].GetVec3();
        }

        if (jsonObj.Has("Rotation"))
        {
            r = jsonObj["Rotation"].GetFloat(r);
        }
        SetRotation(r);
        SetPosition(p);
        SetScale(s);
    }

    void Transform::Serialize(JsonDocument& json) const
    {
        json.WriteString("Type", "Transform");
        json.BeginObject("Data");

        json.BeginArray("Position");
        json.WriteFloat(mPosition.x);
        json.WriteFloat(mPosition.y);
        json.WriteFloat(mPosition.z);
        json.EndArray();

        json.BeginArray("Scale");
        json.WriteFloat(mScale.x);
        json.WriteFloat(mScale.y);
        json.WriteFloat(mScale.z);
        json.EndArray();

        json.WriteFloat("Rotation", mRotation);
        json.EndObject();  // end Data
    }

    glm::mat4 Transform::GetLocalMatrix()
    {
        if (!mIsDirty)
            return mLocalMatrix;
        glm::mat4 S = glm::scale(glm::mat4(1.0f), mScale);
        // origin of mesh is top-left, so have to translate origin to center when rotating
        glm::mat4 R =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.5f * mScale.x, 0.5f * mScale.y, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), mRotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f * mScale.x, -0.5f * mScale.y, 0.0f));
        glm::mat4 T = glm::translate(glm::mat4(1.0f), mPosition);
        mLocalMatrix = T * R * S;
        return mLocalMatrix;
    }

    glm::vec3 Transform::GetRight()
    {
        glm::mat4 m = GetLocalMatrix();
        return glm::normalize(glm::vec3(m[0]));
    }

    glm::vec3 Transform::GetUp()
    {
        glm::mat4 m = GetLocalMatrix();
        return glm::normalize(glm::vec3(m[1]));
    }
}  // namespace mts
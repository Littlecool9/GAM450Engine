/**
 * @file Transform.cpp
 * @author sumin.park
 * @brief  Transform Component
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#include "pch.h"
#include "Transform.h"

namespace rm {
    Transform::Transform(GameObject* owner) 
    : Component(owner), position(0.5), scale(1), rotation(0), isDirty(true) {
    }

    void Transform::Init() {
    }

    void Transform::Update(float dt) {
    }

    void Transform::LateUpdate(float dt) {
    }

    void Transform::Render() {
    }

    void Transform::SetPosition(const math::Vec3& pos)
    {
        position = pos;
        isDirty = true;
    }
    const math::Vec3& Transform::GetPosition() const
    {
        return position;
    }
    void Transform::SetScale(const math::Vec3& s)
    {
        scale = s;
        isDirty = true;
    }
    const math::Vec3& Transform::GetScale() const
    {
        return scale;
    }
    void Transform::SetRotation(float r)
    {
        rotation = r;
        isDirty = true;
    }
    const float Transform::GetRotation() const
    {
        return rotation;
    }


    void Transform::Deserialize(JsonValue const& jsonObj)
    {
        math::Vec3 p(0);
        math::Vec3 s(0);
        float r(0);

        if (jsonObj.Has("Position"))
        {
            JsonValue t = jsonObj["Position"];
            if (t.IsArray() && t.Size() >= 3)
            {
                p.x = t[0].GetFloat(p.x);
                p.y = t[1].GetFloat(p.y);
                p.z = t[2].GetFloat(p.z);
            }
        }

        if (jsonObj.Has("Scale"))
        {
            JsonValue sc = jsonObj["Scale"];
            if (sc.IsArray() && sc.Size() >= 3)
            {
                s.x = sc[0].GetFloat(s.x);
                s.y = sc[1].GetFloat(s.y);
                s.z = sc[2].GetFloat(s.z);
            }
        }

        if (jsonObj.Has("Rotation"))
        {
            r = jsonObj["Rotation"].GetFloat(r);
        }
        SetRotation(r);
        SetPosition(p);
        SetScale(s);
    }

    math::Mat3 Transform::LocalMatrix() const {
        //todo: create Rotate, Scale, Translate functions inside Mat
        //todo: change this to Mat4

        float c = cos(rotation), s = sin(rotation);
        math::Mat3 R(c, s, 0, -s, c, 0, 0, 0, 1);
        math::Mat3 S(scale.x, 0, 0, 0, scale.y, 0, 0, 0, 1);
        math::Mat3 T(1, 0, 0, 0, 1, 0, position.x, position.y, 1);
        return T * R * S;
    }
}  // namespace rm
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

namespace mts
{
    Transform::Transform(GameObject* owner) : Component(owner), position(0.5), scale(1), rotation(0), isDirty(true)
    {
    }

    Transform::Transform() : position(0.5), scale(1), rotation(0)
    {
    }

    void Transform::Init()
    {
    }

    void Transform::Update(float dt)
    {
    }

    void Transform::LateUpdate(float dt)
    {
    }

    void Transform::Render()
    {
    }

    void Transform::SetPosition(const glm::vec3& pos)
    {
        position = pos;
        isDirty = true;
    }
    const glm::vec3& Transform::GetPosition() const
    {
        return position;
    }
    void Transform::SetScale(const glm::vec3& s)
    {
        scale = s;
        isDirty = true;
    }
    const glm::vec3& Transform::GetScale() const
    {
        return scale;
    }
    void Transform::SetRotation(float r)
    {
        rotation = r;
        isDirty = true;
    }
    float Transform::GetRotation() const
    {
        return rotation;
    }

    void Transform::Deserialize(JsonValue const& jsonObj)
    {
        glm::vec3 p(0);
        glm::vec3 s(0);
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

    void Transform::Serialize(JsonDocument& json) const
    {

        json.WriteString("Type", "Transform");
        json.BeginObject("Data"); 


        json.BeginArray("Position");
        json.WriteFloat(position.x);
        json.WriteFloat(position.y);
        json.WriteFloat(position.z);
        json.EndArray();

        json.BeginArray("Scale");
        json.WriteFloat(scale.x);
        json.WriteFloat(scale.y);
        json.WriteFloat(scale.z);
        json.EndArray();

        json.WriteFloat("Rotation", rotation);
        json.EndObject();  // end Data
    }

    glm::mat4 Transform::LocalMatrix() const
    {
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        // origin of mesh is top-left, so have to translate origin to center when rotating
        glm::mat4 R = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f * scale.x, 0.5f * scale.y, 0.0f)) * 
            glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f,0.0f,1.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f * scale.x, -0.5f * scale.y, 0.0f));
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        return T * R * S;
    }
}  // namespace mts
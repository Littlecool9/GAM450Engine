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
    : Component(owner),
        position(0.5), scale(1), rotation(0) {
    }

    void Transform::Init() {
    }

    void Transform::Update(float dt) {
    }

    void Transform::LateUpdate(float dt) {
    }

    void Transform::Render() {
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
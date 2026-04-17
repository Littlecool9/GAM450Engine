/**
 * @file PhysicsCommand2D.h
 * @author sumin.park
 * @brief  Physics commands for physics systems to queue
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <glm/vec2.hpp>
namespace mts
{
    class RigidBody2D;
}

namespace mts::physics
{
    enum class PhysicsCmdType2D
    {
        AddForce,           // continuous force
        AddImpulse,         // instant linear impulse
        AddTorque,          // continuous rotational force
        AddAngularImpulse,  // instant angular impulse
        SetVelocity,
        SetAngularVelocity,
        Teleport  // warp pos(clear forces and accelerations)
    };

    struct PhysicsCommand2D
    {
        PhysicsCmdType2D type;
        RigidBody2D* rb;

        glm::vec2 v;
        float w;
    };
}  // namespace mts::physics
/**
 * @file RigidBody2DDrawer.cpp
 * @author sumin.park
 * @brief  drawer for rigidbody2d
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "RigidBody2DDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"

namespace mts
{
    static const char* BodyTypeToStr(mts::RigidBody2D::BodyType t)
    {
        using BT = mts::RigidBody2D::BodyType;
        switch (t)
        {
            case BT::Static:
                return "Static";
            case BT::Dynamic:
                return "Dynamic";
            case BT::Kinematic:
                return "Kinematic";
            default:
                return "Unknown";
        }
    }

    bool RigidBody2DDrawer::Draw(GameObject* go)
    {
        auto* rb = go->GetComponent<mts::RigidBody2D>();
        if (!rb)
            return false;

        ImGui::PushID("RigidBody2DDrawer");

        if (ImGui::CollapsingHeader("RigidBody2D", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // BodyType dropdown
            int typeIdx = 0;
            auto t = rb->GetBodyType();
            if (t == mts::RigidBody2D::BodyType::Static)
                typeIdx = 0;
            else if (t == mts::RigidBody2D::BodyType::Dynamic)
                typeIdx = 1;
            else
                typeIdx = 2;

            const char* items[] = {"Static", "Dynamic", "Kinematic"};
            if (ImGui::Combo("Body Type", &typeIdx, items, IM_ARRAYSIZE(items)))
            {
                rb->SetBody(static_cast<mts::RigidBody2D::BodyType>(typeIdx));
            }

            // Mass / inertia (only meaningful for Dynamic typically, but we still allow editing)
            float mass = rb->GetMass();
            if (ImGui::DragFloat("Mass", &mass, 0.05f, 0.0f, 100000.0f))
                rb->SetMass(mass);

            float inertia = rb->GetInertia();
            if (ImGui::DragFloat("Inertia", &inertia, 0.05f, 0.0f, 100000.0f))
                rb->SetInertia(inertia);

            // Velocities
            glm::vec2 v = rb->GetVelocity();
            if (ImGui::DragFloat2("Linear Velocity", &v.x, 0.05f))
                rb->SetVelocity(v);

            float av = rb->GetAngularVelocity();
            if (ImGui::DragFloat("Angular Velocity", &av, 0.05f))
                rb->SetAngularVelocity(av);

            // Damping / gravity
            float ld = rb->GetLinearDamping();
            if (ImGui::SliderFloat("Linear Damping", &ld, 0.0f, 10.0f))
                rb->SetLinearDamping(ld);

            float ad = rb->GetAngularDamping();
            if (ImGui::SliderFloat("Angular Damping", &ad, 0.0f, 10.0f))
                rb->SetAngularDamping(ad);

            float gs = rb->GetGravityScale();
            if (ImGui::DragFloat("Gravity Scale", &gs, 0.05f, 0.0f, 100.0f))
                rb->SetGravityScale(gs);

            // Optional small helpers
            if (ImGui::Button("Zero Velocity"))
            {
                rb->SetVelocity(glm::vec2(0.0f));
                rb->SetAngularVelocity(0.0f);
            }
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts
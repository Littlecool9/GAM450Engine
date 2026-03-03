/**
 * @file MovablePlatform.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "MovablePlatform.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"

void MovablePlatform::OnCreate()
{
    InitPlatform();
}

void MovablePlatform::OnUpdate(float dt)
{
    // Detect movement states Fire delegates when movement starts or completes
    UpdateStatus();
}


void MovablePlatform::OnCollisionStay2D(const mts::Collision2D& collision)
{
    if (isMoving)
    {
        mts::GameObject& player = *(collision.other->GetGameObject());
        if (player.HasTag("Player") || player.GetName() == "Player")
        {
            HandlePlayerWhileMoving(player);
        }
    }
}

void MovablePlatform::InitPlatform()
{
    m_parent_actionList = GetGameObject()->GetComponent<mts::ActionList>();
    if (!m_parent_actionList)
    {
        LOG_ERROR("[MovablePlatform] ActionList component not found on GameObject '{}'",
                  GetGameObject()->GetName());
    }

    m_parent_transform = GetGameObject()->GetComponent<mts::Transform>();
    if (!m_parent_transform)
    {
        LOG_ERROR("[MovablePlatform] Transform component not found on GameObject '{}'",
                  GetGameObject()->GetName());
        return;
    }
    currentPosition = m_parent_transform->GetPosition();
    targetPosition = currentPosition; 
}

void MovablePlatform::FirePlatformTranslate(const glm::vec3& targetPos, float timeToFinish)
{
    if (m_parent_actionList != nullptr)
    {
        targetPosition = targetPos;
        m_parent_actionList->Clear();
        m_parent_actionList->AddTranslateAction(targetPos,
                                                GetGameObject(),
                                                timeToFinish,
                                                0,
                                                0,
                                                true,
                                                mts::BaseAction::EaseType::InAndOut);
    }
    else
    {
        LOG_ERROR("[MovablePlatform] ActionList component not found on GameObject '{}'",
                  GetGameObject()->GetName());
    }
}

void MovablePlatform::UpdateStatus()
{
    if (m_parent_transform != nullptr)
    {
        currentPosition = m_parent_transform->GetPosition();
    }

    // Check movement status
    if (isMoving && targetPosition != currentPosition)
    {
        if (glm::distance(currentPosition, targetPosition) < 0.01f)
        {
            currentPosition = targetPosition;
            isMoving = false;
            onMoveComplete.Invoke();
            return;
        }
    }
    if (!isMoving && glm::distance(targetPosition, currentPosition) > 0.01f)
    {
        isMoving = true;
        onMoveStart.Invoke();
        return;
    }
}

void MovablePlatform::HandlePlayerWhileMoving(mts::GameObject& player)
{
    if (!m_parent_transform)
    {
        LOG_ERROR("[MovablePlatform] Transform component not found on platform GameObject");
        return;
    }

    glm::vec3 newPlatformPosition = m_parent_transform->GetPosition();
    glm::vec3 platformDelta = newPlatformPosition - currentPosition;

    // Apply delta to player
    mts::Transform* playerTransform = player.GetComponent<mts::Transform>();
    if (playerTransform)
    {
        glm::vec3 newPlayerPosition = playerTransform->GetPosition() + platformDelta;
        playerTransform->SetPosition(newPlayerPosition);
    }
    else
    {
        LOG_ERROR("[MovablePlatform] Transform component not found on player GameObject");
    }

    // ✅ Update currentPosition AFTER calculating delta
    currentPosition = newPlatformPosition;
}

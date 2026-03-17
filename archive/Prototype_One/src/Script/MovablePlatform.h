/**
 * @file MovablePlatform.h
 * @author Jackey Cen
 * @brief: This is a script that attached to a platform and allows moving to the target position at
 * call
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class ActionList;
    class GameObject;
    class Transform;
}

class MovablePlatform : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;

    void OnCollisionStay2D(const mts::Collision2D& collision) override;

    SCRIPT_CLASS(MovablePlatform);

    // void SetTargetPosition(const glm::vec3& targetPos);
    void FirePlatformTranslate(const glm::vec3& targetPos, float speed);

    mts::Delegate<> onMoveComplete;  // Delegate to notify when movement is complete
    mts::Delegate<> onMoveStart;     // Delegate to notify when movement starts
   private:
    // Helper functions
    void InitPlatform();
    void UpdateStatus();
    void HandlePlayerWhileMoving(mts::GameObject& player);

   private:
    mts::ActionList* m_parent_actionList = nullptr;
    mts::Transform* m_parent_transform = nullptr;

    bool isMoving = false;
    glm::vec3 currentPosition;
    glm::vec3 targetPosition;
};

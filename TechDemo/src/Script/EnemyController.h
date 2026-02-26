/**
 * @file EnemyController.h
 * @author sumin.park
 * @brief  Enemy behavior script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/Scripting/ScriptBase.h"

class EnemyController : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;
    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;

    void OnCollisionEnter2D(const mts::GameObject*) override;

    SCRIPT_CLASS(EnemyController)

   private:
    void QueueLoop();

   private:
    mts::ActionList* mActions = nullptr;

    glm::vec3 mPointA, mPointB, originScale;
    bool mTowardB = true;
    float mMoveTime = 1.5f;

    float mSpinDegrees = 360.0f;
};

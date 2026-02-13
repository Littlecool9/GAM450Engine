/**
 * @file EnemyController.cpp
 * @author sumin.park
 * @brief  Enemy behavior script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyController.h"

void EnemyController::OnCreate()
{
    // todo: ActionList deserialization....
    mActions = GetGameObject()->GetComponent<mts::ActionList>();
    if (!mActions)
        mActions = GetGameObject()->AddComponent<mts::ActionList>();

    mPointA = GetGameObject()->GetComponent<mts::Transform>()->GetPosition();
    mPointB = mPointA + glm::vec3{10.0f, 0.0f, 0.0f};
    originScale = GetGameObject()->GetComponent<mts::Transform>()->GetScale();
    mTowardB = true;
    QueueLoop();
}

void EnemyController::OnUpdate(float dt)
{
    if (mActions && mActions->IsEmpty())
    {
        mTowardB = !mTowardB;
        QueueLoop();
    }
}

void EnemyController::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("moveTime"))
        mMoveTime = fields["moveTime"].GetFloat(mMoveTime);

    if (fields.Has("spinDegrees"))
        mSpinDegrees = fields["spinDegrees"].GetFloat(mSpinDegrees);
}

void EnemyController::OnCollisionEnter2D(const mts::GameObject* game_object)
{
}

void EnemyController::QueueLoop()
{
    glm::vec3 target = mTowardB ? mPointB : mPointA;
    float rotate = mTowardB ? mSpinDegrees : 0;
    ;
    mActions->AddTranslateAction(target,
                                 GetGameObject(),
                                 mMoveTime,
                                 0,
                                 0.0f,
                                 false,
                                 mts::BaseAction::EaseType::Linear);

    mActions->AddRotateAction(rotate,
                              GetGameObject(),
                              mMoveTime,
                              1,
                              0.0f,
                              false,
                              mts::BaseAction::EaseType::Linear);
    mActions->AddScaleAction(originScale * 2.0f, GetGameObject(), mMoveTime, 2, 0, true);
    mActions->AddScaleAction(originScale, GetGameObject(), mMoveTime, 2, 0, true);
}
/**
 * @file RotateAction.cpp
 * @author Dong Ta
 * @brief  Functionality for rotate action
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */
#include "pch.h"
#include "RotateAction.h"

#include "Engine/EntitySystem/Component/Transform.h"

namespace mts
{
    RotateAction::RotateAction(float Goal, GameObject* tar, int id, float lifeSpan, float delay, bool block, EaseType ease)
    {
        this->SetGroupID(id);
        this->SetLifeSpan(lifeSpan);
        this->SetDelay(delay);
        this->SetBlock(block);
        this->SetEaseType(ease);
        this->SetTarget(tar);
        startRotation = 0.0f;
        targetRotation = Goal;
    }
    BaseAction::ActionResult RotateAction::Update(float dt)
    {
        if (!Target)
        {
            return ActionResult::Failed;
        }
        if (Delay <= 0)
        {
            if (FirstTime)
            {
                startRotation = Target->GetComponent<Transform>()->GetRotation();

                FirstTime = false;
            }
            // Vector3 angle = StartAngle + (EndAngle - StartAngle) * Percent;
            float ang = startRotation + (targetRotation - startRotation) * Percent;

            Target->GetComponent<Transform>()->SetRotation(ang);
            if (Percent >= 1)
            {
                return ActionResult::Success;
            }
            else
            {
                return ActionResult::OnGoing;
            }
        }
        else
        {
            Delay -= dt;
            return ActionResult::OnGoing;
        }
    }
}
/**
 * @file ColliderScaleAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "ColliderScaleAction.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"


namespace mts
{
    ColliderScaleAction::ColliderScaleAction(glm::vec2 TargetSize,
                                   GameObject* tar,
                                   int id,
                                   float lifeSpan,
                                   float delay,
                                   bool block,
                                   EaseType ease)
    {
        this->SetGroupID(id);
        this->SetLifeSpan(lifeSpan);
        this->SetDelay(delay);
        this->SetBlock(block);
        this->SetEaseType(ease);
        this->SetTarget(tar);
        targetSize = TargetSize;
    }

	BaseAction::ActionResult ColliderScaleAction::Update(float dt)
	{
        if (!Target)
        {
            return ActionResult::Failed;
        }
        if (Delay <= 0)
        {
            if (FirstTime)
            {
                startSize = Target->GetComponent<BoxCollider2D>()->GetLocalSize();

                FirstTime = false;
            }
            // Vector3 angle = StartAngle + (EndAngle - StartAngle) * Percent;
            glm::vec2 size = startSize + (targetSize - startSize) * Percent;

            Target->GetComponent<BoxCollider2D>()->SetLocalWidth(size.x);
            Target->GetComponent<BoxCollider2D>()->SetLocalHeight(size.y);
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
}  // namespace mts

#include "pch.h"
#include "TranslateAction.h"

#include "Engine/EntitySystem/Component/Transform.h"

namespace mts
{
    TranslateAction::TranslateAction(glm::vec3 Goal,
                                     GameObject* tar,
                                     int id,
                                     float lifeSpan,
                                     float delay,
                                     bool block,
                                     EaseType ease)
        : BaseAction(), startPosition(0), targetPosition(Goal)
    {
        this->SetGroupID(id);
        this->SetLifeSpan(lifeSpan);
        this->SetDelay(delay);
        this->SetBlock(block);
        this->SetEaseType(ease);
        this->SetTarget(tar);
    }

    BaseAction::ActionResult TranslateAction::Update(float dt)
    {
        if (!Target)
        {
            return ActionResult::Failed;
        }
        if (Delay <= 0)
        {
            if (FirstTime)
            {
                startPosition = Target->GetComponent<Transform>()->GetPosition();

                FirstTime = false;
            }
            glm::vec3 pos = startPosition + (targetPosition - startPosition) * Percent;
            Target->GetComponent<Transform>()->SetPosition(pos);
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
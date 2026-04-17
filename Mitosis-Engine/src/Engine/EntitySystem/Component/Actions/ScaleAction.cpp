/**
 * @file ScaleAction.cpp
 * @author Dong Ta
 * @brief  functionality for scale action
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */
#include "pch.h"
#include "ScaleAction.h"

#include "Engine/EntitySystem/Component/Transform.h"


namespace mts
{
    ScaleAction::ScaleAction(glm::vec3 Goal,
                                  GameObject* tar,
                                  int id,
                                  float lifeSpan,
                                  float delay,
                                  bool block,
                                  EaseType ease)
        : BaseAction(), startScale(0), targetScale(Goal)
    {
        this->SetGroupID(id);
        this->SetLifeSpan(lifeSpan);
        this->SetDelay(delay);
        this->SetBlock(block);
        this->SetEaseType(ease);
        this->SetTarget(tar);
    }
    BaseAction::ActionResult ScaleAction::Update(float dt)
    {
        if (!Target)
        {
            return ActionResult::Failed;
        }
        if (Delay <= 0)
        {
            if (FirstTime)
            {
                startScale = Target->GetComponent<Transform>()->GetScale();
                FirstTime = false;
            }
            glm::vec3 scale = startScale + (targetScale - startScale) * Percent;
            Target->GetComponent<Transform>()->SetScale(scale);
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
/**
 * @file ColliderScaleAction.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/Actions/BaseAction.h"


namespace mts
{
    class BoxCollider2D;
    class ColliderScaleAction: public BaseAction
    {
       public:
        ColliderScaleAction(glm::vec2 TargetSize,
                            GameObject* tar,
                            int id,
                            float lifeSpan,
                            float delay,
                            bool block,
                            EaseType ease);
        ActionResult Update(float dt) override;
       private:
        float startWidth;
        float startHeight;
        glm::vec2 startSize;
        glm::vec2 targetSize;
        float goalWidth;
        float goalHeight;

    };

}  // namespace mts
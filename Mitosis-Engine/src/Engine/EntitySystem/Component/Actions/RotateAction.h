

#pragma once
#include "BaseAction.h"

namespace mts
{
    class RotateAction : public BaseAction
    {
       public:
        float startRotation;
        float targetRotation;
        RotateAction(float Goal,
                     GameObject* target,
                     int id,
                     float lifeSpan,
                     float delay,
                     bool block,
                     EaseType ease);
        ActionResult Update(float dt) override;
    };

}

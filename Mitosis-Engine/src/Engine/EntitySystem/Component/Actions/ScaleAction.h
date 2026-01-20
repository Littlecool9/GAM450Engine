#pragma once
#include "BaseAction.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Math/Vec3.h"


namespace mts
{
    class ScaleAction : public BaseAction
    {
       public:
        math::Vec3 startScale;
        math::Vec3 targetScale; 
        ScaleAction(math::Vec3 Goal,
                    GameObject* target,
                    int id,
                    float lifeSpan,
                    float delay,
                    bool block,
                    EaseType ease);
        ActionResult Update(float dt) override;
    };

}

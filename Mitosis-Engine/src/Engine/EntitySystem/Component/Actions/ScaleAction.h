#pragma once
#include <glm/vec3.hpp>

#include "BaseAction.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"


namespace mts
{
    class ScaleAction : public BaseAction
    {
       public:
        glm::vec3 startScale;
        glm::vec3 targetScale; 
        ScaleAction(glm::vec3 Goal,
                    GameObject* target,
                    int id,
                    float lifeSpan,
                    float delay,
                    bool block,
                    EaseType ease);
        ActionResult Update(float dt) override;
    };

}

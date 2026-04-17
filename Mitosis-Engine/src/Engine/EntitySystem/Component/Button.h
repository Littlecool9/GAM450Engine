/**
 * @file Button.h
 * @author Dong Ta
 * @brief  UI Button component. Requires UITransform.
 *         Hit area = UITransform scale half-extents.
 *         Subscribe to delegates at runtime; they are cleared on Clone().
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Component.h"
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Button : public Component
    {
       public:
        enum class State { Idle, Hovered, Pressed };

        void Init() override;
        void Update(float dt) override;
        void Serialize(JsonDocument& doc) const override;
        void Deserialize(JsonValue const& jsonObj) override;
        Component* Clone(GameObject* newOwner) const override;

        State GetState() const { return mState; }

        Delegate<> mOnHoverEnter;  // Idle → Hovered
        Delegate<> mOnHoverExit;   // Hovered/Pressed → Idle
        Delegate<> mOnClick;       // released inside boundary

       private:
        State mState{State::Idle};
    };

}  // namespace mts

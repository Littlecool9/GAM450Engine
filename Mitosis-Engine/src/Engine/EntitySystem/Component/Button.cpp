/**
 * @file Button.cpp
 * @author Dong Ta
 * @brief  UI Button component. Requires UITransform.
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */
#include "pch.h"
#include "Button.h"

#include "UITransform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"
#include "Engine/Renderer.h"

namespace mts
{
    void Button::Init()
    {
        if (!GetGameObject()->GetComponent<UITransform>())
            LOG_WARN("Button on '{}': UITransform missing — button will not function.",
                     GetGameObject()->GetName().c_str());
    }

    void Button::Update(float dt)
    {
        auto* uit = GetGameObject()->GetComponent<UITransform>();
        if (!uit)
            return;

        glm::vec2 center   = uit->GetNormalizedCenter();
        glm::vec3 effScale = uit->GetEffectiveScale();
        glm::vec2 mouse    = Renderer::MouseToUISpace();

        // Convert effective scale from UI units to normalized viewport fraction
        Viewport gv = Renderer::GetGameViewport();
        float vpW = (gv.valid ? static_cast<float>(gv.w)
                               : static_cast<float>(Renderer::GetWindowWidth())) / PixelsPerUnit;
        float vpH = (gv.valid ? static_cast<float>(gv.h)
                               : static_cast<float>(Renderer::GetWindowHeight())) / PixelsPerUnit;

        float halfW = (effScale.x / vpW) * 0.5f;
        float halfH = (effScale.y / vpH) * 0.5f;

        bool inside = mouse.x >= center.x - halfW && mouse.x <= center.x + halfW
                   && mouse.y >= center.y - halfH && mouse.y <= center.y + halfH;

        State prev = mState;

        if (!inside)
        {
            mState = State::Idle;
        }
        else if (Input::IsMouseDown(MouseButton::Left))
        {
            mState = State::Pressed;
        }
        else
        {
            if (prev == State::Pressed && Input::IsMouseReleased(MouseButton::Left))
                mOnClick.Invoke();
            mState = State::Hovered;
        }

        if (prev == State::Idle && mState == State::Hovered)
            mOnHoverEnter.Invoke();
        else if (prev != State::Idle && mState == State::Idle)
            mOnHoverExit.Invoke();
    }

    void Button::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "Button");
        doc.BeginObject("Data");
        doc.EndObject();
    }

    void Button::Deserialize(JsonValue const& jsonObj) {}

    Component* Button::Clone(GameObject* newOwner) const
    {
        Button* c = new Button(*this);
        c->mOnHoverEnter.Clear();
        c->mOnHoverExit.Clear();
        c->mOnClick.Clear();
        c->InitFromClone(newOwner);
        return c;
    }

}  // namespace mts

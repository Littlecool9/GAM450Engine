/**
 * @file InputGuidanceScript.cpp
 * @author rahul
 * @brief  Shows or hides the SpriteRenderer based on whether a gamepad is connected.
 *         Keyboard guidance objects have ForController = false (visible when no pad).
 *         Controller guidance objects have ForController = true (visible when pad connected).
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "InputGuidanceScript.h"

#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"
#include "Engine/Serialization/JsonAbstraction.h"

void InputGuidanceScript::Init()
{
    mRenderer = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    if (!mRenderer)
    {
        LOG_WARN("InputGuidanceScript: no SpriteRenderer on '{}'",
                 GetGameObject()->GetName().c_str());
        return;
    }

    const bool connected = mts::Input::IsGamepadConnected();
    ApplyVisibility(connected);
    mPrevControllerState = connected;
}

void InputGuidanceScript::OnUpdate(float /*dt*/)
{
    if (!mRenderer)
        return;

    const bool connected = mts::Input::IsGamepadConnected();
    if (connected == mPrevControllerState)
        return;

    mPrevControllerState = connected;
    ApplyVisibility(connected);
}

void InputGuidanceScript::ApplyVisibility(bool connected)
{
    mRenderer->SetAlpha(connected == mForController ? 1.0f : 0.0f);
}

void InputGuidanceScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("ForController"))
        mForController = fields["ForController"].GetBool(mForController);
}

void InputGuidanceScript::SerializeFields(mts::JsonDocument& out) const
{
    if (mForController)
        out.WriteBool("ForController", mForController);
}

void InputGuidanceScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "For Controller",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mForController}});
}

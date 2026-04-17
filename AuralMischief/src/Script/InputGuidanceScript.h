/**
 * @file InputGuidanceScript.h
 * @author rahul
 * @brief  Shows or hides the SpriteRenderer based on whether a gamepad is connected.
 *         Keyboard guidance objects have ForController = false (visible when no pad).
 *         Controller guidance objects have ForController = true (visible when pad connected).
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class SpriteRenderer;
}  // namespace mts

class InputGuidanceScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(InputGuidanceScript)

    void Init() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

   private:
    void ApplyVisibility(bool controllerConnected);

    mts::SpriteRenderer* mRenderer{nullptr};
    bool mPrevControllerState{false};

    // false = keyboard guidance (visible when no gamepad)
    // true  = controller guidance (visible when gamepad connected)
    bool mForController{false};
};

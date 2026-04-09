/**
 * @file VolumeStepButton.h
 * @brief Button script that steps master volume by a serialized Delta on click.
 *        The same class serves both the "−" and "+" buttons; each instance
 *        stores its own Delta (negative or positive) in the scene JSON.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Button;
}

class VolumeStepButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(VolumeStepButton)

    void Init() override;
    void OnEnable() override;
    void OnDisable() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

   private:
    float mDelta{0.1f};

    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
};

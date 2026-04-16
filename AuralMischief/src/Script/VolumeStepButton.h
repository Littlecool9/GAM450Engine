/**
 * @file VolumeStepButton.h
 * @author Jackey Cen
 * @brief Button script that steps master volume by a serialized Delta on click.
 *        The same class serves both the "−" and "+" buttons; each instance
 *        stores its own Delta (negative or positive) in the scene JSON.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <string>

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
    void Awake() override;
    void OnEnable() override;
    void OnDisable() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

   private:
    void UpdateTextDisplay(float volume);

    float mDelta{0.1f};
    int mCategory{0};  // mts::AudioCategory: 0=Master,1=Music,2=SFX,3=UI,4=Ambience
    // std::string mTextGoName;   // GO whose SpriteRenderer shows volume %
    mts::GameObject* mBindingGO{nullptr};
    mts::UUID mBindingUUID =
        mts::UUID::InvalidID;  // serialized UUID for inspector binding and scene JSON

    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
};

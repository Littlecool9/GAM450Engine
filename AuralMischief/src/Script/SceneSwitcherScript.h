/**
 * @file SceneSwitcherScript.h
 * @author sumin.park
 * @brief  switches scene on button click
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Button;
}

class SceneSwitcherScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(SceneSwitcherScript)

    void Init() override;
    void Awake() override;
    void OnDisable() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

   private:
    int mSceneIndex{0};

    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
};

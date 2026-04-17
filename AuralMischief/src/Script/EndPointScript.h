/**
 * @file EndPointScript.h
 * @author sumin.park
 * @brief  trigger win
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class EndPointScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(EndPointScript)
    void OnCollisionEnter2D(const mts::Collision2D& collision) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

   private:
    int mTargetSceneIndex{2};
};

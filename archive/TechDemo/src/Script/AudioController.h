/**
 * @file AudioController.h
 * @author sumin.park
 * @brief  script for audio controller
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class AudioController : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    SCRIPT_CLASS(AudioController)

   private:
    float mMasterVolume   = 1.0f;
    float mMusicVolume    = 1.0f;
    float mSFXVolume      = 1.0f;
    float mUIVolume       = 1.0f;
    float mAmbienceVolume = 1.0f;
};
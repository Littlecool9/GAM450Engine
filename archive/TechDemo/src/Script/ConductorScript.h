/**
 * @file ConductorScript.h
 * @author rahul
 * @brief NativeScript that pushes rhythm parameters (BPM, BeatsPerBar, OffsetSec)
 *        into ConductorSystem, making the conductor data-driven from JSON.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */
#pragma once
#include "Engine/Scripting/ScriptBase.h"

class ConductorScript : public mts::ScriptBase
{
public:
    void OnCreate() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;

    SCRIPT_CLASS(ConductorScript)

private:
    float mBPM         = 120.0f;
    int   mBeatsPerBar = 4;
    float mOffsetSec   = 0.0f;
};

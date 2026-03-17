/**
 * @file ConductorScript.cpp
 * @author rahul
 * @brief See ConductorScript.h
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */
#include "pch.h"
#include "ConductorScript.h"
#include "Engine/Conductor/ConductorSystem.h"

void ConductorScript::OnCreate()
{
    mts::ConductorSystem& conductor = mts::ConductorSystem::Get();
    conductor.SetBPM(mBPM);
    conductor.SetBeatsPerBar(mBeatsPerBar);
    conductor.SetOffsetSec(mOffsetSec);
}

void ConductorScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("BPM"))
        mBPM = fields["BPM"].GetFloat(mBPM);
    if (fields.Has("BeatsPerBar"))
        mBeatsPerBar = fields["BeatsPerBar"].GetInt(mBeatsPerBar);
    if (fields.Has("OffsetSec"))
        mOffsetSec = fields["OffsetSec"].GetFloat(mOffsetSec);
}

void ConductorScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("BPM", mBPM);
    out.WriteInt("BeatsPerBar", mBeatsPerBar);
    out.WriteFloat("OffsetSec", mOffsetSec);
}

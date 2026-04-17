/**
 * @file AudioController.cpp
 * @author sumin.park
 * @brief  script for audio controller
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "AudioController.h"
#include "../AuralMischief.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Serialization/JsonAbstraction.h"

void AudioController::Init()
{
    if (AuralMischief::Get()->HasAppliedInitialVolumes())
        return;

    auto& audio = mts::AudioManager::Get();
    audio.SetCategoryVolume(mts::AudioCategory::Master, mMasterVolume);
    audio.SetCategoryVolume(mts::AudioCategory::Music, mMusicVolume);
    audio.SetCategoryVolume(mts::AudioCategory::SFX, mSFXVolume);
    audio.SetCategoryVolume(mts::AudioCategory::UI, mUIVolume);
    audio.SetCategoryVolume(mts::AudioCategory::Ambience, mAmbienceVolume);
    AuralMischief::Get()->MarkInitialVolumesApplied();
}

void AudioController::Awake()
{
    if (!mMusicEventPath.empty())
    {
        mts::MusicTrack track;
        track.trackId = "music";
        track.eventId = mMusicEventPath;
        track.bpm = mBpm;
        track.beatsPerBar = mBeatsPerBar;
        track.offsetSec = mOffsetSec;
        mts::AudioManager::Get().StartMusic(track);
    }
}

void AudioController::OnDisable()
{
    mts::AudioManager::Get().StopMusic();
}

void AudioController::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("MasterVolume"))
        mMasterVolume = fields["MasterVolume"].GetFloat(mMasterVolume);
    if (fields.Has("MusicVolume"))
        mMusicVolume = fields["MusicVolume"].GetFloat(mMusicVolume);
    if (fields.Has("SFXVolume"))
        mSFXVolume = fields["SFXVolume"].GetFloat(mSFXVolume);
    if (fields.Has("UIVolume"))
        mUIVolume = fields["UIVolume"].GetFloat(mUIVolume);
    if (fields.Has("AmbienceVolume"))
        mAmbienceVolume = fields["AmbienceVolume"].GetFloat(mAmbienceVolume);
    if (fields.Has("MusicEventPath"))
        mMusicEventPath = fields["MusicEventPath"].GetString("");
    if (fields.Has("Bpm"))
        mBpm = fields["Bpm"].GetFloat(mBpm);
    if (fields.Has("BeatsPerBar"))
        mBeatsPerBar = fields["BeatsPerBar"].GetInt(mBeatsPerBar);
    if (fields.Has("OffsetSec"))
        mOffsetSec = fields["OffsetSec"].GetFloat(mOffsetSec);
}

void AudioController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("MasterVolume", mMasterVolume);
    out.WriteFloat("MusicVolume", mMusicVolume);
    out.WriteFloat("SFXVolume", mSFXVolume);
    out.WriteFloat("UIVolume", mUIVolume);
    out.WriteFloat("AmbienceVolume", mAmbienceVolume);
    out.WriteString("MusicEventPath", mMusicEventPath.c_str());
    out.WriteFloat("Bpm", mBpm);
    out.WriteInt("BeatsPerBar", mBeatsPerBar);
    out.WriteFloat("OffsetSec", mOffsetSec);
}

void AudioController::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Master Volume",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mMasterVolume},
                      .step = 0.05f});
    fields.push_back({.name = "Music Volume",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mMusicVolume},
                      .step = 0.05f});
    fields.push_back({.name = "SFX Volume",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mSFXVolume},
                      .step = 0.05f});
    fields.push_back({.name = "UI Volume",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mUIVolume},
                      .step = 0.05f});
    fields.push_back({.name = "Ambience Volume",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAmbienceVolume},
                      .step = 0.05f});
    fields.push_back(
        {.name = "BPM", .type = mts::ScriptField::Type::Float, .ptr = {.f = &mBpm}, .step = 1.0f});
    fields.push_back({.name = "Beats Per Bar",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mBeatsPerBar},
                      .step = 1});
    fields.push_back({.name = "Offset Sec",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOffsetSec},
                      .step = 0.01f});
}

/**
 * @file AudioController.cpp
 * @author sumin.park
 * @brief  script for audio controller
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AudioController.h"
#include "Engine/Audio/AudioManager.h"

void AudioController::OnCreate()
{
    auto& audio = mts::AudioManager::Get();
    audio.SetCategoryVolume(mts::AudioCategory::Master, mMasterVolume);
    audio.SetCategoryVolume(mts::AudioCategory::Music, mMusicVolume);
    audio.SetCategoryVolume(mts::AudioCategory::SFX, mSFXVolume);
    audio.SetCategoryVolume(mts::AudioCategory::UI, mUIVolume);
    audio.SetCategoryVolume(mts::AudioCategory::Ambience, mAmbienceVolume);
}

void AudioController::OnUpdate(float dt)
{
    auto& audio = mts::AudioManager::Get();
    audio.SetCategoryVolume(mts::AudioCategory::Master, mMasterVolume);
    audio.SetCategoryVolume(mts::AudioCategory::Music, mMusicVolume);
    audio.SetCategoryVolume(mts::AudioCategory::SFX, mSFXVolume);
    audio.SetCategoryVolume(mts::AudioCategory::UI, mUIVolume);
    audio.SetCategoryVolume(mts::AudioCategory::Ambience, mAmbienceVolume);
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
}

void AudioController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("MasterVolume", mMasterVolume);
    out.WriteFloat("MusicVolume", mMusicVolume);
    out.WriteFloat("SFXVolume", mSFXVolume);
    out.WriteFloat("UIVolume", mUIVolume);
    out.WriteFloat("AmbienceVolume", mAmbienceVolume);
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
}

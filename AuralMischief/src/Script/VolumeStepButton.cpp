/**
 * @file VolumeStepButton.cpp
 * @brief Button script that steps master volume by a serialized Delta on click.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "VolumeStepButton.h"

#include <algorithm>

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

void VolumeStepButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("VolumeStepButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }
}

void VolumeStepButton::Awake()
{
    if (!mButton)
        return;
    if (mBindingUUID != mts::UUID::InvalidID)
    {
        if (auto* scene = mts::SceneSystem::GetCurrentScene())
        {
            mBindingGO = scene->FindGameObjectByUUID(mBindingUUID);
            if (!mBindingGO)
            {
                LOG_WARN(
                    "VolumeStepButton: binding GO with UUID {} not found in scene",
                    mBindingUUID.ToString().c_str());
            }
        }
    }
}

void VolumeStepButton::OnEnable()
{
    // Subscribe in OnEnable (not Awake) so that SetActive(true) after a hide
    // re-wires the click handler after OnDisable() unsubscribed it.
    if (!mButton)
        return;

    mClickId = mButton->mOnClick.Subscribe(
        [this]()
        {
            const auto cat = static_cast<mts::AudioCategory>(mCategory);
            auto& audio = mts::AudioManager::Get();
            const float current = audio.GetCategoryVolume(cat);
            const float next = std::clamp(current + mDelta, 0.0f, 1.0f);
            audio.SetCategoryVolume(cat, next);
            UpdateTextDisplay(next);
        });

    // Show current volume text when the menu first appears.
    const auto cat = static_cast<mts::AudioCategory>(mCategory);
    UpdateTextDisplay(mts::AudioManager::Get().GetCategoryVolume(cat));
}

void VolumeStepButton::OnDisable()
{
    if (mButton)
        mButton->mOnClick.Unsubscribe(mClickId);
    mClickId = 0;
}

void VolumeStepButton::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("Delta"))
        mDelta = fields["Delta"].GetFloat(mDelta);
    if (fields.Has("Category"))
        mCategory = fields["Category"].GetInt(mCategory);
    //if (fields.Has("TextGoName"))
    //    mTextGoName = fields["TextGoName"].GetString("");
    if (fields.Has("TextGoUUID"))
        mBindingUUID = mts::UUID::FromString(fields["TextGoUUID"].GetString());
}

void VolumeStepButton::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("Delta", mDelta);
    out.WriteInt("Category", mCategory);
    //out.WriteString("TextGoName", mTextGoName.c_str());
    out.WriteString("TextGoUUID", mBindingUUID.ToString().c_str());
}

void VolumeStepButton::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Delta",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDelta},
                      .step = 0.05f});
    fields.push_back({.name = "Category (0=Master,2=SFX)",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mCategory},
                      .step = 1.0f});
    //fields.push_back({.name = "Text GO Name",
    //                  .type = mts::ScriptField::Type::String,
    //                  .ptr = {.s = &mTextGoName}});
    fields.push_back({.name = "Text GO UUID",
                      .type = mts::ScriptField::Type::GameObject,
                      .ptr = {.go = &mBindingGO},
                      .uuid = &mBindingUUID});
}

void VolumeStepButton::UpdateTextDisplay(float volume)
{
    //if (mTextGoName.empty())
    //    return;
    if (!mBindingGO)
    {
        if (mBindingUUID == mts::UUID::InvalidID)
            return;
        mBindingGO = mts::SceneSystem::GetCurrentScene()->FindGameObjectByUUID(mBindingUUID);
    }
    if (!mBindingGO)
        return;

    auto* sr = mBindingGO->GetComponent<mts::SpriteRenderer>();
    if (!sr)
        return;

    int pct = static_cast<int>(volume * 100.0f + 0.5f);
    sr->SetText(std::to_string(pct) + "%");
}

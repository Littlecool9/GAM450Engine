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
#include "Engine/EntitySystem/GameObject/GameObject.h"

void VolumeStepButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("VolumeStepButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
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
            auto& audio = mts::AudioManager::Get();
            const float current =
                audio.GetCategoryVolume(mts::AudioCategory::Master);
            const float next = std::clamp(current + mDelta, 0.0f, 1.0f);
            audio.SetCategoryVolume(mts::AudioCategory::Master, next);
            LOG_INFO("VolumeStepButton: master volume {} -> {}", current, next);
        });
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
}

void VolumeStepButton::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("Delta", mDelta);
}

void VolumeStepButton::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "Delta",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDelta},
                      .step = 0.05f});
}

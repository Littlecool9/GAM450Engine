/**
 * @file ButtonTest.cpp
 * @author Dong Ta
 * @brief  Test script — logs to console when the Button on this GameObject is clicked.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PauseButton.h"

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Time/Time.h"

void PauseButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("PauseButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
        return;
    }
    mAudio = &mts::AudioManager::Get();
    if (!mAudio->isInitialized())
    {
        LOG_WARN("PauseButton: AudioManager not initialized");
    }
}

void PauseButton::Awake()
{
    if (mButton)
        mClickId = mButton->mOnClick.Subscribe([this]() { TogglePause(); });
}

void PauseButton::OnUpdate(float dt)
{
    (void)dt;

    if (mts::Input::IsKeyPressed(mts::Key::Escape))
        TogglePause();
}

void PauseButton::OnDisable()
{
    if (mPausedByThis)
    {
        const float restore = (mStoredTimeScale > 0.0f) ? mStoredTimeScale : 1.0f;
        mts::Time::SetTimeScale(restore);
        if (mAudio)
            mAudio->ResumeAll();
        mPausedByThis = false;
    }

    if (mButton)
    {
        mButton->mOnHoverEnter.Unsubscribe(mHoverEnterId);
        mButton->mOnHoverExit.Unsubscribe(mHoverExitId);
        mButton->mOnClick.Unsubscribe(mClickId);
    }
}

void PauseButton::TogglePause()
{
    if (!mPausedByThis)
    {
        const float current = mts::Time::GetTimeScale();
        mStoredTimeScale = (current > 0.0f) ? current : 1.0f;
        mts::Time::SetTimeScale(0.0f);
        mPausedByThis = true;
        LOG_INFO("PauseButton: game paused");
        if (mAudio)
            mAudio->PauseAll();
        return;
    }

    const float restore = (mStoredTimeScale > 0.0f) ? mStoredTimeScale : 1.0f;
    mts::Time::SetTimeScale(restore);
    if (mAudio)
    {
        mAudio->ResumeAll();  // In case any audio was paused by the time scale change
    }
    mPausedByThis = false;
    LOG_INFO("PauseButton: game resumed (timeScale={})", restore);
}

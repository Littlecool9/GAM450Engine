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

#include "PauseMenuPanel.h"

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Time/Time.h"

void PauseButton::Awake()
{
    if (!mMenuPanel)
    {
        LOG_WARN(
            "PauseButton: PauseMenuPanel not found in scene — pause menu "
            "will not be shown");
    }
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
    if (auto* scene = mts::SceneSystem::GetCurrentScene())
    {
        if (auto* panelGO = scene->FindGameObjectByName("PauseMenuPanel"))
            mMenuPanel = panelGO->GetScript<PauseMenuPanel>();
    }
    if (mButton)
        mClickId = mButton->mOnClick.Subscribe([this]() { TogglePause(); });
}

void PauseButton::OnUpdate(float dt)
{
    (void)dt;

    if (mts::Input::IsKeyPressed(mts::Key::Escape) ||
        mts::Input::IsGamepadButtonPressed(mts::GamepadButton::Start))
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
        if (mMenuPanel)
            mMenuPanel->SetVisible(false);
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
        if (mMenuPanel)
            mMenuPanel->SetVisible(true);
        return;
    }

    const float restore = (mStoredTimeScale > 0.0f) ? mStoredTimeScale : 1.0f;
    mts::Time::SetTimeScale(restore);
    if (mAudio)
    {
        mAudio->ResumeAll();  // In case any audio was paused by the time scale change
    }
    if (mMenuPanel)
        mMenuPanel->SetVisible(false);
    mPausedByThis = false;
    LOG_INFO("PauseButton: game resumed (timeScale={})", restore);
}

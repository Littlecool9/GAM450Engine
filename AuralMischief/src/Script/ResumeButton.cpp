/**
 * @file ResumeButton.cpp
 * @author Jackey Cen
 * @brief Resume button on the pause menu.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "ResumeButton.h"

#include "PauseButton.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Time/Time.h"

void ResumeButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("ResumeButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }

    // The pause button GameObject in CurrentScene is named "ESC" (it uses the
    // PauseButton script). Locate it so we can delegate the toggle to the same
    // code path that ESC and the header icon use.
    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (scene)
    {
        auto* pauseGO = scene->FindGameObjectByName("ESC");
        mPauseButton = pauseGO ? pauseGO->GetScript<PauseButton>() : nullptr;
    }

    if (!mPauseButton)
    {
        LOG_WARN(
            "ResumeButton: could not locate PauseButton script in scene — "
            "falling back to direct time/audio restore");
    }
}

void ResumeButton::OnEnable()
{
    if (!mButton)
        return;

    mClickId = mButton->mOnClick.Subscribe(
        [this]()
        {
            if (mPauseButton)
            {
                mPauseButton->TogglePause();
                return;
            }

            // Defensive fallback if PauseButton wasn't found.
            mts::Time::SetTimeScale(1.0f);
            mts::AudioManager::Get().ResumeAll();
        });
}

void ResumeButton::OnUpdate(float dt)
{
    if (mts::Input::IsGamepadButtonPressed(mts::GamepadButton::B))
    {
        if (mButton)
            mButton->mOnClick.Invoke();
    }
}

void ResumeButton::OnDisable()
{
    if (mButton)
        mButton->mOnClick.Unsubscribe(mClickId);
    mClickId = 0;
}

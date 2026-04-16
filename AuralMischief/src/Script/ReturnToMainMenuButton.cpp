/**
 * @file ReturnToMainMenuButton.cpp
 * @author Jackey Cen
 * @brief Return-to-main-menu button on the pause menu.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ReturnToMainMenuButton.h"

#include "PauseMenuNavigator.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Time/Time.h"

namespace
{
    // Main menu scene index, per Data/Properties/SceneRegistry.json.
    constexpr int kMainMenuSceneIndex = 1;
}  // namespace

void ReturnToMainMenuButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("ReturnToMainMenuButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }

    // Cache the navigator so the click can route through the confirm dialog
    // instead of switching scenes immediately.
    if (auto* scene = mts::SceneSystem::GetCurrentScene())
    {
        if (auto* panel = scene->FindGameObjectByName("PauseMenuPanel"))
            mNavigator = panel->GetScript<PauseMenuNavigator>();
    }
    if (!mNavigator)
    {
        LOG_WARN(
            "ReturnToMainMenuButton: no PauseMenuNavigator found — "
            "click will fall back to immediate scene switch");
    }
}

void ReturnToMainMenuButton::OnEnable()
{
    if (!mButton)
        return;

    mClickId = mButton->mOnClick.Subscribe(
        [this]()
        {
            if (mNavigator)
            {
                mNavigator->ShowReturnConfirm();
                return;
            }

            // Defensive fallback: if the navigator is missing for any reason,
            // preserve the original behavior so the button isn't dead.
            mts::Time::SetTimeScale(1.0f);
            mts::AudioManager::Get().ResumeAll();
            mts::SceneSystem::SwitchScene(kMainMenuSceneIndex);
        });
}

void ReturnToMainMenuButton::OnDisable()
{
    if (mButton)
        mButton->mOnClick.Unsubscribe(mClickId);
    mClickId = 0;
}

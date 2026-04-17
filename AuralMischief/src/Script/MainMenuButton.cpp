/**
 * @file MainMenuButton.cpp
 * @author Jackey Cen
 * @brief  Script that switches to the gameplay scene when the main menu button is clicked.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "MainMenuButton.h"

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/SceneSystem.h"

void MainMenuButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("MainMenuButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }
}

void MainMenuButton::Awake()
{
    if (mButton)
    {
        mClickId = mButton->mOnClick.Subscribe([this]() { mts::SceneSystem::SwitchScene(6); });
    }
}

void MainMenuButton::OnDisable()
{
    if (mButton)
    {
        mButton->mOnClick.Unsubscribe(mClickId);
    }
}

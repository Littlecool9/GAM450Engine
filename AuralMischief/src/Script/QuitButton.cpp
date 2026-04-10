/**
 * @file MainMenuButton.cpp
 * @brief  Script that switches to the gameplay scene when the main menu button is clicked.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "QuitButton.h"


#include "Engine/AppSystem/AppSystem.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/SceneSystem.h"

void QuitButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("QuitButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }
}

void QuitButton::Awake()
{
    if (mButton)
    {
        mClickId = mButton->mOnClick.Subscribe(
            [this]() { mts::AppSystem::Get().Quit(); });
    }
}

void QuitButton::OnDisable()
{
    if (mButton)
    {
        mButton->mOnClick.Unsubscribe(mClickId);
    }
}

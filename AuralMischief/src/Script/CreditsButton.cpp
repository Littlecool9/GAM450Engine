/**
 * @file CreditsButton.cpp
 * @author Jackey Cen
 * @brief  Script that switches to the gameplay scene when the main menu button is clicked.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "CreditsButton.h"

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/SceneSystem.h"

void CreditsButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("CreditsButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }
}

void CreditsButton::Awake()
{
    if (mButton)
    {
        mClickId = mButton->mOnClick.Subscribe([this]() { mts::SceneSystem::SwitchScene(5); });
    }
}

void CreditsButton::OnDisable()
{
    if (mButton)
    {
        mButton->mOnClick.Unsubscribe(mClickId);
    }
}

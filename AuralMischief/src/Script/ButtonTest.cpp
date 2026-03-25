/**
 * @file ButtonTest.cpp
 * @author Dong Ta
 * @brief  Test script — logs to console when the Button on this GameObject is clicked.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ButtonTest.h"

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

void ButtonTest::OnCreate()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("ButtonTest: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
        return;
    }

    mHoverEnterId = mButton->mOnHoverEnter.Subscribe(
        [this]() { LOG_INFO("ButtonTest: '{}' hover enter", GetGameObject()->GetName().c_str()); });

    mHoverExitId = mButton->mOnHoverExit.Subscribe(
        [this]() { LOG_INFO("ButtonTest: '{}' hover exit", GetGameObject()->GetName().c_str()); });

    mClickId = mButton->mOnClick.Subscribe(
        [this]() { LOG_INFO("ButtonTest: '{}' clicked!", GetGameObject()->GetName().c_str()); });
}

void ButtonTest::OnDisable()
{
    if (mButton)
    {
        mButton->mOnHoverEnter.Unsubscribe(mHoverEnterId);
        mButton->mOnHoverExit.Unsubscribe(mHoverExitId);
        mButton->mOnClick.Unsubscribe(mClickId);
    }
}

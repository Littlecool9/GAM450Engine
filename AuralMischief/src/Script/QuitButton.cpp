/**
 * @file QuitButton.cpp
 * @brief  Main-menu Quit button. Shows a confirmation dialog before quitting.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "QuitButton.h"

#include "Engine/AppSystem/AppSystem.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

void QuitButton::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("QuitButton: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }

    if (auto* scene = mts::SceneSystem::GetCurrentScene())
    {
        mConfirmBg     = scene->FindGameObjectByName("MainMenu_Confirm_Background");
        mConfirmPrompt = scene->FindGameObjectByName("MainMenu_Confirm_Prompt");
        mConfirmYes    = scene->FindGameObjectByName("MainMenu_Confirm_Yes");
        mConfirmNo     = scene->FindGameObjectByName("MainMenu_Confirm_No");

        // The Yes/No buttons sit on top of the main-menu buttons — their hit
        // areas overlap. Cache the siblings so we can disable them while the
        // dialog is open and restore them when the player clicks "No".
        const char* kSiblingNames[] = {
            "StartButton", "HowToButton", "OptionsButton", "CreditsButton",
        };
        for (const char* name : kSiblingNames)
        {
            if (auto* go = scene->FindGameObjectByName(name))
            {
                if (auto* btn = go->GetComponent<mts::Button>())
                    mSiblingButtons.push_back(btn);
            }
        }
    }

    if (mConfirmYes)
        mYesButton = mConfirmYes->GetComponent<mts::Button>();
    if (mConfirmNo)
        mNoButton = mConfirmNo->GetComponent<mts::Button>();

    const bool confirmReady = mConfirmBg && mConfirmPrompt && mConfirmYes && mConfirmNo
                              && mYesButton && mNoButton;
    if (!confirmReady)
    {
        LOG_WARN("QuitButton: confirm dialog GOs missing — click will fall back "
                 "to immediate quit");
    }

    // Scene JSON doesn't serialize the active flag, so hide the confirm dialog
    // here on every scene load.
    HideConfirm();
}

void QuitButton::OnEnable()
{
    if (!mButton)
        return;

    const bool confirmReady = mConfirmBg && mConfirmPrompt && mConfirmYes && mConfirmNo
                              && mYesButton && mNoButton;

    if (!confirmReady)
    {
        mClickId = mButton->mOnClick.Subscribe(
            [this]() { mts::AppSystem::Get().Quit(); });
        return;
    }

    mClickId = mButton->mOnClick.Subscribe([this]() { ShowConfirm(); });
    mYesClickId = mYesButton->mOnClick.Subscribe(
        []() { mts::AppSystem::Get().Quit(); });
    mNoClickId = mNoButton->mOnClick.Subscribe([this]() { HideConfirm(); });
}

void QuitButton::OnDisable()
{
    if (mButton)
        mButton->mOnClick.Unsubscribe(mClickId);
    if (mYesButton)
        mYesButton->mOnClick.Unsubscribe(mYesClickId);
    if (mNoButton)
        mNoButton->mOnClick.Unsubscribe(mNoClickId);

    mClickId = 0;
    mYesClickId = 0;
    mNoClickId = 0;
}

void QuitButton::ShowConfirm()
{
    if (mConfirmBg) mConfirmBg->SetActive(true);
    if (mConfirmPrompt) mConfirmPrompt->SetActive(true);
    if (mConfirmYes) mConfirmYes->SetActive(true);
    if (mConfirmNo) mConfirmNo->SetActive(true);

    // Suppress underlying main-menu buttons so clicks on Yes/No don't also
    // trigger the overlapping button beneath them.
    if (mButton) mButton->SetEnabled(false);
    for (auto* btn : mSiblingButtons)
        if (btn) btn->SetEnabled(false);
}

void QuitButton::HideConfirm()
{
    if (mConfirmBg) mConfirmBg->SetActive(false);
    if (mConfirmPrompt) mConfirmPrompt->SetActive(false);
    if (mConfirmYes) mConfirmYes->SetActive(false);
    if (mConfirmNo) mConfirmNo->SetActive(false);

    if (mButton) mButton->SetEnabled(true);
    for (auto* btn : mSiblingButtons)
        if (btn) btn->SetEnabled(true);
}

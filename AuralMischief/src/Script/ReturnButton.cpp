/**
 * @file ReturnButton.cpp
 * @author Jackey Cen
 * @brief  Script for a button that returns to a parent menu panel when clicked; also listens for
 * ESC key to trigger click. Used for the back button on the pause menu and the instruction screen.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ReturnButton.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"

#include "UIPanels.h"

void ReturnButton::OnEnable()
{
    LOG_INFO("ReturnButton Awake: looking for Button component on '{}'",
             GetGameObject()->GetName().c_str());
    if (!mButton)
    {
        mButton = GetGameObject()->GetComponent<mts::Button>();
        if (!mButton)
            LOG_WARN("ReturnButton: no Button component found on '{}'",
                     GetGameObject()->GetName().c_str());
    }
    if (kMainMenuButtonUUIDs.size() != mMainMenuButtons.size())
    {
        for (const auto& uuid : kMainMenuButtonUUIDs)
        {
            auto* scene = mts::SceneSystem::GetCurrentScene();
            if (!scene)
                return;
            auto* go = scene->FindGameObjectByUUID(uuid);
            if (go)
            {
                auto* btn = go->GetComponent<mts::Button>();
                if (btn)
                    mMainMenuButtons.push_back(btn);
                else
                    LOG_WARN("ReturnButton: main menu button '{}' does not have a Button component",
                             go->GetName().c_str());
            }
            else
                LOG_WARN("ReturnButton: main menu button '{}' not found in scene", uuid.ToString());
        }
    }
    if (mButton)
        mClickId = mButton->mOnClick.Subscribe(
            [this]()
            {
                if (this->mMenuPanel)
                    this->mMenuPanel->SetPanelActive(false);
                for (auto* btn : mMainMenuButtons)
                {
                    if (btn)
                        btn->SetEnabled(true);
                }
            });

    // Hover highlight — cache the sprite's current tint as the baseline so we
    // can restore it on hover-exit without relying on the sprite being untouched.
    if (!mSprite)
        mSprite = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    if (mSprite && !mHoverApplied)
        mOriginalTint = mSprite->GetTintColor();

    if (mButton)
    {
        mHoverEnterId = mButton->mOnHoverEnter.Subscribe(
            [this]()
            {
                if (!mSprite || mHoverApplied)
                    return;
                mSprite->SetTintColor(mOriginalTint * mHoverTint);
                mHoverApplied = true;
            });
        mHoverExitId = mButton->mOnHoverExit.Subscribe(
            [this]()
            {
                if (!mSprite || !mHoverApplied)
                    return;
                mSprite->SetTintColor(mOriginalTint);
                mHoverApplied = false;
            });
    }
}

void ReturnButton::OnUpdate(float dt)
{
    if (mts::Input::IsKeyPressed(mts::Key::Escape) ||
        mts::Input::IsGamepadButtonPressed(mts::GamepadButton::B))
    {
        if (mButton)
            mButton->mOnClick.Invoke();
    }
}

void ReturnButton::OnDisable()
{
    if (mButton)
    {
        mButton->mOnClick.Unsubscribe(mClickId);
        mButton->mOnHoverEnter.Unsubscribe(mHoverEnterId);
        mButton->mOnHoverExit.Unsubscribe(mHoverExitId);
    }
    if (mSprite && mHoverApplied)
    {
        mSprite->SetTintColor(mOriginalTint);
        mHoverApplied = false;
    }
}

void ReturnButton::DeserializeFields(mts::JsonValue const& fields)
{
    // Deserialize the UUIDs of the main menu buttons to enable when this return button is clicked
    if (fields.Has("MainMenuButtons") && fields["MainMenuButtons"].IsArray())
    {
        const auto& buttonsArray = fields["MainMenuButtons"];
        size_t buttonCount = buttonsArray.Size();
        mMainMenuButtons.clear();
        auto* scene = mts::SceneSystem::GetCurrentScene();
        if (!scene)
            return;
        for (size_t i = 0; i < buttonCount; ++i)
        {
            const auto& buttonJson = buttonsArray[i];
            kMainMenuButtonUUIDs.emplace_back(mts::UUID::FromString(buttonJson.GetString()));
        }
    }

    if (fields.Has("HoverTint"))
    {
        mts::JsonValue tint = fields["HoverTint"];
        mHoverTint = tint.GetVec3(mHoverTint);
    }
}

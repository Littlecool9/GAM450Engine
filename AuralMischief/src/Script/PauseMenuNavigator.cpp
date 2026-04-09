/**
 * @file PauseMenuNavigator.cpp
 * @brief Keyboard navigation, focus signifier, and confirm dialog logic for
 *        the pause menu.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PauseMenuNavigator.h"

#include <algorithm>

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/Time/Time.h"

namespace
{
    // Main menu scene index, per Data/Properties/SceneRegistry.json. Mirrors
    // ReturnToMainMenuButton.cpp.
    constexpr int kMainMenuSceneIndex = 1;

    // Slot indices into mMainItems for clarity.
    constexpr int kMainVolume = 0;
    constexpr int kMainResume = 1;
    constexpr int kMainReturn = 2;

    constexpr int kConfirmYes = 0;
    constexpr int kConfirmNo = 1;
}  // namespace

void PauseMenuNavigator::Awake()
{
    BuildItems();

    // Hide the confirm dialog at startup so it only appears when the player
    // explicitly opens it from the Return button.
    for (auto* go : mConfirmMembers)
    {
        if (go)
            go->SetActive(false);
    }

    // Wire mouse-hover to keyboard focus so the highlight follows the cursor
    // and the two input modes never disagree.
    for (size_t i = 0; i < mMainItems.size(); ++i)
        SubscribeHover(mMainItems[i], Group::Main, static_cast<int>(i));
    for (size_t i = 0; i < mConfirmItems.size(); ++i)
        SubscribeHover(mConfirmItems[i], Group::Confirm, static_cast<int>(i));

    // The Yes/No buttons have no dedicated script — the navigator owns their
    // click handlers so all confirm-related logic stays in one place.
    if (mConfirmItems.size() > kConfirmYes &&
        !mConfirmItems[kConfirmYes].buttons.empty() &&
        mConfirmItems[kConfirmYes].buttons[0])
    {
        auto* yesBtn = mConfirmItems[kConfirmYes].buttons[0];
        const auto id = yesBtn->mOnClick.Subscribe(
            []()
            {
                // Restore time/audio before the scene swap so the main menu
                // does not load frozen.
                mts::Time::SetTimeScale(1.0f);
                mts::AudioManager::Get().ResumeAll();
                mts::SceneSystem::SwitchScene(kMainMenuSceneIndex);
            });
        mClickSubs.push_back({yesBtn, id});
    }

    if (mConfirmItems.size() > kConfirmNo &&
        !mConfirmItems[kConfirmNo].buttons.empty() &&
        mConfirmItems[kConfirmNo].buttons[0])
    {
        auto* noBtn = mConfirmItems[kConfirmNo].buttons[0];
        const auto id = noBtn->mOnClick.Subscribe(
            [this]() { HideReturnConfirm(); });
        mClickSubs.push_back({noBtn, id});
    }
}

void PauseMenuNavigator::OnDisable()
{
    for (auto& sub : mHoverSubs)
    {
        if (sub.button)
            sub.button->mOnHoverEnter.Unsubscribe(sub.id);
    }
    mHoverSubs.clear();

    for (auto& sub : mClickSubs)
    {
        if (sub.button)
            sub.button->mOnClick.Unsubscribe(sub.id);
    }
    mClickSubs.clear();

    ClearHighlight();
}

void PauseMenuNavigator::OnUpdate(float /*dt*/)
{
    if (!mPanelVisible)
        return;

    using mts::Input;
    using mts::Key;

    if (mActiveGroup == Group::Main)
    {
        if (Input::IsKeyPressed(Key::Up))
            SetMainCursor(mMainCursor - 1);
        else if (Input::IsKeyPressed(Key::Down))
            SetMainCursor(mMainCursor + 1);

        if (mMainCursor == kMainVolume)
        {
            // Volume slot — Left/Right step master volume directly via the
            // existing button click handlers.
            if (Input::IsKeyPressed(Key::Left) &&
                mMainItems[kMainVolume].buttons.size() > 0 &&
                mMainItems[kMainVolume].buttons[0])
            {
                mMainItems[kMainVolume].buttons[0]->mOnClick.Invoke();
            }
            else if (Input::IsKeyPressed(Key::Right) &&
                     mMainItems[kMainVolume].buttons.size() > 1 &&
                     mMainItems[kMainVolume].buttons[1])
            {
                mMainItems[kMainVolume].buttons[1]->mOnClick.Invoke();
            }
            // Space/Enter intentionally do nothing here — there is no single
            // "activate" action for a stepper.
        }
        else
        {
            if (Input::IsKeyPressed(Key::Space) ||
                Input::IsKeyPressed(Key::Enter))
            {
                ActivateFocused();
            }
        }
    }
    else  // Group::Confirm
    {
        if (Input::IsKeyPressed(Key::Left))
            SetConfirmCursor(kConfirmYes);
        else if (Input::IsKeyPressed(Key::Right))
            SetConfirmCursor(kConfirmNo);

        if (Input::IsKeyPressed(Key::Space) ||
            Input::IsKeyPressed(Key::Enter))
        {
            ActivateFocused();
        }
    }
}

void PauseMenuNavigator::OnPanelShown()
{
    mPanelVisible = true;
    mActiveGroup = Group::Main;
    mMainCursor = 0;
    ApplyHighlight();
}

void PauseMenuNavigator::OnPanelHidden()
{
    // Make sure no stale dialog or highlight survives a hide/show cycle.
    HideReturnConfirm();
    ClearHighlight();
    mPanelVisible = false;
}

void PauseMenuNavigator::ShowReturnConfirm()
{
    ClearHighlight();
    for (auto* go : mConfirmMembers)
    {
        if (go)
            go->SetActive(true);
    }
    mActiveGroup = Group::Confirm;
    mConfirmCursor = kConfirmNo;  // default to the safe choice
    ApplyHighlight();
}

void PauseMenuNavigator::HideReturnConfirm()
{
    if (mActiveGroup != Group::Confirm)
        return;

    ClearHighlight();
    for (auto* go : mConfirmMembers)
    {
        if (go)
            go->SetActive(false);
    }
    mActiveGroup = Group::Main;
    ApplyHighlight();
}

void PauseMenuNavigator::BuildItems()
{
    mMainItems.clear();
    mConfirmItems.clear();
    mConfirmMembers.clear();

    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (!scene)
    {
        LOG_WARN("PauseMenuNavigator: no current scene during Awake");
        return;
    }

    mMainItems.push_back(MakeVolumeItem());
    mMainItems.push_back(MakeItem("PauseMenu_Resume"));
    mMainItems.push_back(MakeItem("PauseMenu_ReturnToMenu"));

    mConfirmItems.push_back(MakeItem("PauseMenu_Confirm_Yes"));
    mConfirmItems.push_back(MakeItem("PauseMenu_Confirm_No"));

    static constexpr const char* kConfirmGoNames[] = {
        "PauseMenu_Confirm_Background",
        "PauseMenu_Confirm_Prompt",
        "PauseMenu_Confirm_Yes",
        "PauseMenu_Confirm_No",
    };
    for (const char* name : kConfirmGoNames)
    {
        auto* go = scene->FindGameObjectByName(name);
        if (go)
            mConfirmMembers.push_back(go);
        else
            LOG_WARN("PauseMenuNavigator: confirm member '{}' not found", name);
    }
}

PauseMenuNavigator::FocusItem PauseMenuNavigator::MakeItem(const char* name)
{
    FocusItem item;
    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (!scene)
        return item;

    auto* go = scene->FindGameObjectByName(name);
    if (!go)
    {
        LOG_WARN("PauseMenuNavigator: focus item GO '{}' not found", name);
        return item;
    }

    auto* btn = go->GetComponent<mts::Button>();
    auto* sprite = go->GetComponent<mts::SpriteRenderer>();
    if (btn)
        item.buttons.push_back(btn);
    if (sprite)
    {
        item.sprites.push_back(sprite);
        item.originalTints.push_back(sprite->GetTintColor());
    }
    return item;
}

PauseMenuNavigator::FocusItem PauseMenuNavigator::MakeVolumeItem()
{
    FocusItem item;
    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (!scene)
        return item;

    static constexpr const char* kVolumeNames[] = {
        "PauseMenu_VolumeMinus",
        "PauseMenu_VolumePlus",
    };
    for (const char* name : kVolumeNames)
    {
        auto* go = scene->FindGameObjectByName(name);
        if (!go)
        {
            LOG_WARN("PauseMenuNavigator: volume member '{}' not found", name);
            continue;
        }
        if (auto* btn = go->GetComponent<mts::Button>())
            item.buttons.push_back(btn);
        if (auto* sprite = go->GetComponent<mts::SpriteRenderer>())
        {
            item.sprites.push_back(sprite);
            item.originalTints.push_back(sprite->GetTintColor());
        }
    }
    return item;
}

void PauseMenuNavigator::SubscribeHover(FocusItem const& item, Group group,
                                       int index)
{
    for (auto* btn : item.buttons)
    {
        if (!btn)
            continue;
        const auto id = btn->mOnHoverEnter.Subscribe(
            [this, group, index]()
            {
                if (!mPanelVisible)
                    return;
                if (group != mActiveGroup)
                    return;
                if (group == Group::Main)
                    SetMainCursor(index);
                else
                    SetConfirmCursor(index);
            });
        mHoverSubs.push_back({btn, id});
    }
}

void PauseMenuNavigator::ApplyHighlight()
{
    auto& items = (mActiveGroup == Group::Main) ? mMainItems : mConfirmItems;
    const int cursor =
        (mActiveGroup == Group::Main) ? mMainCursor : mConfirmCursor;

    if (cursor < 0 || cursor >= static_cast<int>(items.size()))
        return;

    auto& item = items[cursor];
    for (size_t i = 0; i < item.sprites.size(); ++i)
    {
        if (!item.sprites[i])
            continue;
        const glm::vec3 base = (i < item.originalTints.size())
                                   ? item.originalTints[i]
                                   : glm::vec3(1.0f);
        item.sprites[i]->SetTintColor(base * mHighlightTint);
    }
    mHighlightApplied = true;
}

void PauseMenuNavigator::ClearHighlight()
{
    if (!mHighlightApplied)
        return;

    auto restore = [](FocusItem& item)
    {
        for (size_t i = 0; i < item.sprites.size(); ++i)
        {
            if (!item.sprites[i])
                continue;
            const glm::vec3 base = (i < item.originalTints.size())
                                       ? item.originalTints[i]
                                       : glm::vec3(1.0f);
            item.sprites[i]->SetTintColor(base);
        }
    };
    for (auto& item : mMainItems)
        restore(item);
    for (auto& item : mConfirmItems)
        restore(item);
    mHighlightApplied = false;
}

void PauseMenuNavigator::SetMainCursor(int index)
{
    const int last = static_cast<int>(mMainItems.size()) - 1;
    if (last < 0)
        return;
    index = std::clamp(index, 0, last);
    if (index == mMainCursor && mHighlightApplied)
        return;
    ClearHighlight();
    mMainCursor = index;
    ApplyHighlight();
}

void PauseMenuNavigator::SetConfirmCursor(int index)
{
    const int last = static_cast<int>(mConfirmItems.size()) - 1;
    if (last < 0)
        return;
    index = std::clamp(index, 0, last);
    if (index == mConfirmCursor && mHighlightApplied)
        return;
    ClearHighlight();
    mConfirmCursor = index;
    ApplyHighlight();
}

void PauseMenuNavigator::ActivateFocused()
{
    auto& items = (mActiveGroup == Group::Main) ? mMainItems : mConfirmItems;
    const int cursor =
        (mActiveGroup == Group::Main) ? mMainCursor : mConfirmCursor;
    if (cursor < 0 || cursor >= static_cast<int>(items.size()))
        return;
    auto& item = items[cursor];
    if (item.buttons.empty() || !item.buttons[0])
        return;
    item.buttons[0]->mOnClick.Invoke();
}

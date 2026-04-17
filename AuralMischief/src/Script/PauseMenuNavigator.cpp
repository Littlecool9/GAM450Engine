/**
 * @file PauseMenuNavigator.cpp
 * @author Jackey Cen
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

    constexpr int kConfirmYes = 0;
    constexpr int kConfirmNo = 1;

    // Human-readable names for each binding slot (inspector + log messages).
    constexpr const char* kSlotNames[] = {
        "Volume Minus",
        "Volume Plus",
        "SFX Minus",
        "SFX Plus",
        "Resume",
        "Return",
        "Confirm Background",
        "Confirm Prompt",
        "Confirm Yes",
        "Confirm No",
    };
}  // namespace

// ---------------------------------------------------------------------------
// Serialization
// ---------------------------------------------------------------------------

void PauseMenuNavigator::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("bindings"))
    {
        mts::JsonValue arr = fields["bindings"];
        if (arr.IsArray())
        {
            const size_t count = std::min(arr.Size(), static_cast<size_t>(SlotCount));
            for (size_t i = 0; i < count; ++i)
                mBindingUUIDs[i] = mts::UUID::FromString(arr[i].GetString());
        }
    }
}

void PauseMenuNavigator::SerializeFields(mts::JsonDocument& out) const
{
    out.BeginArray("bindings");
    for (int i = 0; i < SlotCount; ++i)
        out.WriteString(mBindingUUIDs[i].ToString().c_str());
    out.EndArray();
}

void PauseMenuNavigator::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    for (int i = 0; i < SlotCount; ++i)
    {
        fields.push_back({.name = kSlotNames[i],
                          .type = mts::ScriptField::Type::GameObject,
                          .ptr = {.go = &mBindingGOs[i]},
                          .uuid = &mBindingUUIDs[i]});
    }
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void PauseMenuNavigator::Awake()
{
    // Resolve all UUID bindings to GO pointers.
    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (scene)
    {
        for (int i = 0; i < SlotCount; ++i)
        {
            if (mBindingUUIDs[i].IsValid())
            {
                mBindingGOs[i] = scene->FindGameObjectByUUID(mBindingUUIDs[i]);
                if (!mBindingGOs[i])
                    LOG_WARN("PauseMenuNavigator: binding '{}' UUID not found", kSlotNames[i]);
            }
        }
    }

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
    if (mConfirmItems.size() > kConfirmYes && !mConfirmItems[kConfirmYes].buttons.empty() &&
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

    if (mConfirmItems.size() > kConfirmNo && !mConfirmItems[kConfirmNo].buttons.empty() &&
        mConfirmItems[kConfirmNo].buttons[0])
    {
        auto* noBtn = mConfirmItems[kConfirmNo].buttons[0];
        const auto id = noBtn->mOnClick.Subscribe([this]() { HideReturnConfirm(); });
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

    using mts::GamepadButton;
    using mts::Input;
    using mts::Key;

    if (Input::IsGamepadConnected())
    {
        if ((abs(Input::GetGamepadAxis(mts::GamepadAxis::LeftY)) < 0.2f) &&
            (abs(Input::GetGamepadAxis(mts::GamepadAxis::LeftX)) < 0.2f))
        {
            mGamePadLeftNeutral = true;
        }
        if ((abs(Input::GetGamepadAxis(mts::GamepadAxis::RightY)) < 0.2f) &&
            (abs(Input::GetGamepadAxis(mts::GamepadAxis::RightX)) < 0.2f))
        {
            mGamePadRightNeutral = true;
        }
    }

    bool upPressed = Input::IsKeyPressed(Key::Up) ||
                     Input::IsGamepadButtonPressed(GamepadButton::DPadUp) ||
                     (Input::IsGamepadConnected() && mGamePadLeftNeutral &&
                      Input::GetGamepadAxis(mts::GamepadAxis::LeftY) < -0.2f) ||
                     (Input::IsGamepadConnected() && mGamePadRightNeutral &&
                      Input::GetGamepadAxis(mts::GamepadAxis::RightY) < -0.2f);

    bool downPressed = Input::IsKeyPressed(Key::Down) ||
                       Input::IsGamepadButtonPressed(GamepadButton::DPadDown) ||
                       (Input::IsGamepadConnected() && mGamePadLeftNeutral &&
                        Input::GetGamepadAxis(mts::GamepadAxis::LeftY) > 0.2f) ||
                       (Input::IsGamepadConnected() && mGamePadRightNeutral &&
                        Input::GetGamepadAxis(mts::GamepadAxis::RightY) > 0.2f);

    bool rightPressed = Input::IsKeyPressed(Key::Right) ||
                        Input::IsGamepadButtonPressed(GamepadButton::DPadRight) ||
                        (Input::IsGamepadConnected() && mGamePadLeftNeutral &&
                         Input::GetGamepadAxis(mts::GamepadAxis::LeftX) > 0.2f) ||
                        (Input::IsGamepadConnected() && mGamePadRightNeutral &&
                         Input::GetGamepadAxis(mts::GamepadAxis::RightX) > 0.2f);

    bool leftPressed = Input::IsKeyPressed(Key::Left) ||
                       Input::IsGamepadButtonPressed(GamepadButton::DPadLeft) ||
                       (Input::IsGamepadConnected() && mGamePadLeftNeutral &&
                        Input::GetGamepadAxis(mts::GamepadAxis::LeftX) < -0.2f) ||
                       (Input::IsGamepadConnected() && mGamePadRightNeutral &&
                        Input::GetGamepadAxis(mts::GamepadAxis::RightX) < -0.2f);

    if (Input::IsGamepadConnected())
    {
        if ((abs(Input::GetGamepadAxis(mts::GamepadAxis::LeftY)) > 0.2f) ||
            (abs(Input::GetGamepadAxis(mts::GamepadAxis::LeftX)) > 0.2f))
        {
            mGamePadLeftNeutral = false;
        }
        if ((abs(Input::GetGamepadAxis(mts::GamepadAxis::RightY)) > 0.2f) ||
            (abs(Input::GetGamepadAxis(mts::GamepadAxis::RightX)) > 0.2f))
        {
            mGamePadRightNeutral = false;
        }
    }

    if (mActiveGroup == Group::Main)
    {
        if (upPressed)
        {
            SetMainCursor(mMainCursor - 1);
            mts::AudioManager::Get().Play("event:/hero_evade");
        }
        else if (downPressed)
        {
            SetMainCursor(mMainCursor + 1);
            mts::AudioManager::Get().Play("event:/hero_evade");
        }

        if (Input::IsKeyPressed(Key::Space) || Input::IsKeyPressed(Key::Enter) ||
            Input::IsGamepadButtonPressed(GamepadButton::A))
        {
            ActivateFocused();
        }
    }
    else  // Group::Confirm
    {
        if (leftPressed)
            SetConfirmCursor(kConfirmYes);
        else if (rightPressed)
            SetConfirmCursor(kConfirmNo);

        if (Input::IsKeyPressed(Key::Space) || Input::IsKeyPressed(Key::Enter) ||
            Input::IsGamepadButtonPressed(GamepadButton::A))
        {
            ActivateFocused();
        }

        // B button / Escape cancel the confirm dialog and return to main menu items
        if (Input::IsGamepadButtonPressed(GamepadButton::B))
            HideReturnConfirm();
    }
}

// ---------------------------------------------------------------------------
// Panel show / hide
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Confirm dialog
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Item building (uses resolved mBindingGOs)
// ---------------------------------------------------------------------------

void PauseMenuNavigator::BuildItems()
{
    mMainItems.clear();
    mConfirmItems.clear();
    mConfirmMembers.clear();

    // Main menu focus items — each volume/SFX button is its own slot so the
    // pause menu behaves like the main-menu options sub-menu: Up/Down walks
    // through every actionable button, Enter activates the focused one.
    mMainItems.push_back(MakeItem(mBindingGOs[SFXMinus]));
    mMainItems.push_back(MakeItem(mBindingGOs[SFXPlus]));
    mMainItems.push_back(MakeItem(mBindingGOs[VolumeMinus]));
    mMainItems.push_back(MakeItem(mBindingGOs[VolumePlus]));
    mMainItems.push_back(MakeItem(mBindingGOs[Resume]));
    mMainItems.push_back(MakeItem(mBindingGOs[Return]));

    // Confirm dialog focus items
    mConfirmItems.push_back(MakeItem(mBindingGOs[ConfirmYes]));
    mConfirmItems.push_back(MakeItem(mBindingGOs[ConfirmNo]));

    // Confirm dialog members (toggled via SetActive for visibility)
    const Slot confirmSlots[] = {ConfirmBg, ConfirmPrompt, ConfirmYes, ConfirmNo};
    for (Slot s : confirmSlots)
    {
        if (mBindingGOs[s])
            mConfirmMembers.push_back(mBindingGOs[s]);
    }
}

PauseMenuNavigator::FocusItem PauseMenuNavigator::MakeItem(mts::GameObject* go)
{
    FocusItem item;
    if (!go)
        return item;

    if (auto* btn = go->GetComponent<mts::Button>())
        item.buttons.push_back(btn);
    if (auto* sprite = go->GetComponent<mts::SpriteRenderer>())
    {
        item.sprites.push_back(sprite);
        item.originalTints.push_back(sprite->GetTintColor());
    }
    return item;
}

// ---------------------------------------------------------------------------
// Hover / highlight / cursor
// ---------------------------------------------------------------------------

void PauseMenuNavigator::SubscribeHover(FocusItem const& item, Group group, int index)
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
    const int cursor = (mActiveGroup == Group::Main) ? mMainCursor : mConfirmCursor;

    if (cursor < 0 || cursor >= static_cast<int>(items.size()))
        return;

    auto& item = items[cursor];
    for (size_t i = 0; i < item.sprites.size(); ++i)
    {
        if (!item.sprites[i])
            continue;
        const glm::vec3 base =
            (i < item.originalTints.size()) ? item.originalTints[i] : glm::vec3(1.0f);
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
            const glm::vec3 base =
                (i < item.originalTints.size()) ? item.originalTints[i] : glm::vec3(1.0f);
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
    const int cursor = (mActiveGroup == Group::Main) ? mMainCursor : mConfirmCursor;
    if (cursor < 0 || cursor >= static_cast<int>(items.size()))
        return;
    auto& item = items[cursor];
    if (item.buttons.empty() || !item.buttons[0])
        return;
    item.buttons[0]->mOnClick.Invoke();
}

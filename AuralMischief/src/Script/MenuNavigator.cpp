/**
 * @file MenuNavigator.cpp
 * @brief Implementation of the reusable menu focus / highlight script.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "MenuNavigator.h"

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

// ---------------------------------------------------------------------------
// Serialization
// ---------------------------------------------------------------------------

void MenuNavigator::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("buttons"))
    {
        mts::JsonValue arr = fields["buttons"];
        if (arr.IsArray())
        {
            mButtonUUIDs.clear();
            mButtonUUIDs.reserve(arr.Size());
            for (size_t i = 0; i < arr.Size(); ++i)
                mButtonUUIDs.emplace_back(
                    mts::UUID::FromString(arr[i].GetString()));
        }
    }

    if (fields.Has("highlightTint"))
    {
        mts::JsonValue tint = fields["highlightTint"];
        mHighlightTint = tint.GetVec3(mHighlightTint);
    }
}

void MenuNavigator::SerializeFields(mts::JsonDocument& out) const
{
    out.BeginArray("buttons");
    for (auto const& uuid : mButtonUUIDs)
        out.WriteString(uuid.ToString().c_str());
    out.EndArray();

    out.WriteVec3("highlightTint", mHighlightTint);
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void MenuNavigator::OnEnable()
{
    EnsureResolved();

    if (mItems.empty())
        return;

    const int last = static_cast<int>(mItems.size()) - 1;
    if (mCursor < 0 || mCursor > last)
        mCursor = 0;

    SubscribeAll();
    ApplyHighlight();

    mSkipNextUpdate = true;
}

void MenuNavigator::OnDisable()
{
    UnsubscribeAll();
    ClearHighlight();
}

void MenuNavigator::OnUpdate(float /*dt*/)
{
    if (mItems.empty())
        return;

    // If every target button has been disabled (e.g., a sub-panel is covering
    // this menu and called SetEnabled(false) on our buttons), stay dormant so
    // we don't fight another MenuNavigator for the same Up/Down/Enter keys.
    bool anyEnabled = false;
    for (auto const& item : mItems)
    {
        if (item.button && item.button->IsEnabled())
        {
            anyEnabled = true;
            break;
        }
    }
    if (!anyEnabled)
        return;

    if (mSkipNextUpdate)
    {
        mSkipNextUpdate = false;
        return;
    }

    using mts::GamepadButton;
    using mts::Input;
    using mts::Key;

    if (Input::IsKeyPressed(Key::Up) ||
        Input::IsGamepadButtonPressed(GamepadButton::DPadUp))
        StepCursor(-1);
    else if (Input::IsKeyPressed(Key::Down) ||
             Input::IsGamepadButtonPressed(GamepadButton::DPadDown))
        StepCursor(+1);

    if (Input::IsKeyPressed(Key::Space) ||
        Input::IsKeyPressed(Key::Enter) ||
        Input::IsGamepadButtonPressed(GamepadButton::A))
        ActivateFocused();
}

// ---------------------------------------------------------------------------
// Lazy resolution
// ---------------------------------------------------------------------------

void MenuNavigator::EnsureResolved()
{
    if (mResolved)
        return;

    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (!scene)
        return;

    mItems.clear();
    mItems.reserve(mButtonUUIDs.size());

    for (auto const& uuid : mButtonUUIDs)
    {
        auto* go = scene->FindGameObjectByUUID(uuid);
        if (!go)
        {
            LOG_WARN("MenuNavigator: button UUID '{}' not found in scene",
                     uuid.ToString());
            continue;
        }

        auto* btn = go->GetComponent<mts::Button>();
        if (!btn)
        {
            LOG_WARN("MenuNavigator: GO '{}' has no Button, skipping",
                     go->GetName().c_str());
            continue;
        }

        FocusItem item;
        item.button = btn;
        item.sprite = go->GetComponent<mts::SpriteRenderer>();
        if (item.sprite)
            item.originalTint = item.sprite->GetTintColor();
        else
            LOG_WARN("MenuNavigator: GO '{}' has Button but no SpriteRenderer; "
                     "highlight will have no visual effect",
                     go->GetName().c_str());

        mItems.push_back(item);
    }

    mResolved = true;
}

// ---------------------------------------------------------------------------
// Hover / highlight / cursor
// ---------------------------------------------------------------------------

void MenuNavigator::SubscribeAll()
{
    for (size_t i = 0; i < mItems.size(); ++i)
    {
        auto* btn = mItems[i].button;
        if (!btn)
            continue;
        const int index = static_cast<int>(i);
        const auto id = btn->mOnHoverEnter.Subscribe(
            [this, index]() { SetCursor(index); });
        mHoverSubs.push_back({btn, id});
    }
}

void MenuNavigator::UnsubscribeAll()
{
    for (auto& sub : mHoverSubs)
    {
        if (sub.button)
            sub.button->mOnHoverEnter.Unsubscribe(sub.id);
    }
    mHoverSubs.clear();
}

void MenuNavigator::ApplyHighlight()
{
    if (mCursor < 0 || mCursor >= static_cast<int>(mItems.size()))
        return;

    auto& item = mItems[mCursor];
    if (item.sprite)
        item.sprite->SetTintColor(item.originalTint * mHighlightTint);

    mHighlightApplied = true;
}

void MenuNavigator::ClearHighlight()
{
    if (!mHighlightApplied)
        return;

    for (auto& item : mItems)
    {
        if (item.sprite)
            item.sprite->SetTintColor(item.originalTint);
    }
    mHighlightApplied = false;
}

void MenuNavigator::SetCursor(int index)
{
    const int last = static_cast<int>(mItems.size()) - 1;
    if (last < 0)
        return;

    if (index < 0)
        index = 0;
    else if (index > last)
        index = last;

    if (index == mCursor && mHighlightApplied)
        return;

    ClearHighlight();
    mCursor = index;
    ApplyHighlight();
}

void MenuNavigator::StepCursor(int delta)
{
    if (mItems.empty())
        return;
    const int n = static_cast<int>(mItems.size());
    const int next = ((mCursor + delta) % n + n) % n;
    SetCursor(next);
}

void MenuNavigator::ActivateFocused()
{
    if (mCursor < 0 || mCursor >= static_cast<int>(mItems.size()))
        return;
    auto* btn = mItems[mCursor].button;
    if (btn)
        btn->mOnClick.Invoke();
}

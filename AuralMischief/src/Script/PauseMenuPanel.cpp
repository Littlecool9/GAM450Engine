/**
 * @file PauseMenuPanel.cpp
 * @brief Visibility controller for the pause menu.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PauseMenuPanel.h"

#include <cmath>

#include "PauseMenuNavigator.h"

#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/Texture.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace
{
    // Names of the sibling GameObjects that make up the pause menu. Kept
    // in sync with CurrentScene.scene.
    constexpr const char* kMemberNames[] = {
        "PauseMenu_Background",
        "PauseMenu_Instructions",
        "PauseMenu_Instructions_Controller",
        "PauseMenu_VolumeMinus",
        "PauseMenu_VolumePlus",
        "PauseMenu_Master_Volume",
        "PauseMenu_SFX_VolumeDown",
        "PauseMenu_SFX_VolumeUp",
        "PauseMenu_SFX_Volume",
        "PauseMenu_SFX_Prompt",
        "PauseMenu_Resume",
        "PauseMenu_ReturnToMenu",
        "PauseMenu_Volumn_Icon",
    };

    constexpr const char* kInstructionsGoName  = "PauseMenu_Instructions";
    constexpr const char* kKeyboardTexPath     = "Data/Resources/Instructions.tex";
    constexpr const char* kControllerTexPath   = "Data/Resources/InstructionsXbox.tex";

    // Thresholds for gamepad stick / trigger activity. Triggers rest at -1.0
    // and go to +1.0 when fully pressed — so crossing roughly the midpoint is
    // a clear intentional press.
    constexpr float kStickDeadband   = 0.25f;
    constexpr float kTriggerPressed  = 0.0f;
    // Raw mouse coords — a couple of pixels of jitter should not count as
    // "the player is using the mouse".
    constexpr float kMouseMoveEps    = 2.0f;

    // Returns true if any connected gamepad is actively being used this
    // frame. Buttons: any down. Sticks: either axis past the deadband.
    // Triggers: pushed past neutral.
    bool IsGamepadActive()
    {
        for (int pad = 0; pad < mts::Input::MaxGamepads; ++pad)
        {
            if (!mts::Input::IsGamepadConnected(pad))
                continue;

            for (int b = 0; b < static_cast<int>(mts::GamepadButton::Count); ++b)
            {
                if (mts::Input::IsGamepadButtonDown(
                        static_cast<mts::GamepadButton>(b), pad))
                    return true;
            }

            using mts::GamepadAxis;
            const float lx = mts::Input::GetGamepadAxis(GamepadAxis::LeftX, pad);
            const float ly = mts::Input::GetGamepadAxis(GamepadAxis::LeftY, pad);
            const float rx = mts::Input::GetGamepadAxis(GamepadAxis::RightX, pad);
            const float ry = mts::Input::GetGamepadAxis(GamepadAxis::RightY, pad);
            if (std::abs(lx) > kStickDeadband || std::abs(ly) > kStickDeadband ||
                std::abs(rx) > kStickDeadband || std::abs(ry) > kStickDeadband)
                return true;

            const float lt = mts::Input::GetGamepadAxis(GamepadAxis::LeftTrigger, pad);
            const float rt = mts::Input::GetGamepadAxis(GamepadAxis::RightTrigger, pad);
            if (lt > kTriggerPressed || rt > kTriggerPressed)
                return true;
        }
        return false;
    }
}  // namespace

void PauseMenuPanel::Awake()
{
    mMembers.clear();
    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (!scene)
        return;

    for (const char* name : kMemberNames)
    {
        auto* go = scene->FindGameObjectByName(name);
        if (go)
            mMembers.push_back(go);
        else
            LOG_WARN("PauseMenuPanel: member '{}' not found in scene", name);
    }

    // Cache the sibling navigator (lives on this same GameObject as a second
    // NativeScript) so we can notify it when the menu is shown/hidden.
    mNavigator = GetGameObject()->GetScript<PauseMenuNavigator>();
    if (!mNavigator)
    {
        LOG_WARN("PauseMenuPanel: no PauseMenuNavigator script on this GameObject — "
                 "keyboard navigation will be unavailable");
    }

    // Resolve the instructions sprite + load both texture variants so we can
    // hot-swap them as the player alternates between keyboard and controller.
    if (auto* instructionsGo = scene->FindGameObjectByName(kInstructionsGoName))
        mInstructionsSprite = instructionsGo->GetComponent<mts::SpriteRenderer>();

    mKeyboardTex   = mts::ResourceManager::GetTexture(kKeyboardTexPath);
    mControllerTex = mts::ResourceManager::GetTexture(kControllerTexPath);
}

void PauseMenuPanel::Start()
{
    // Hide the menu once all scripts have Awake'd/OnEnable'd and wired up
    // their subscriptions. Hiding earlier would skip those subscriptions.
    SetVisible(false);
}

void PauseMenuPanel::OnUpdate(float /*dt*/)
{
    // Track the most recently used input device even while the panel is
    // hidden so the first frame of a show reflects the correct art.
    InputDevice detected = mActiveDevice;

    if (IsGamepadActive())
    {
        detected = InputDevice::Controller;
    }
    else
    {
        using mts::Input;

        const auto [mx, my] = Input::GetMousePosition();
        const bool mouseMoved =
            mMouseInitialized &&
            (std::abs(mx - mLastMouseX) > kMouseMoveEps ||
             std::abs(my - mLastMouseY) > kMouseMoveEps);
        mLastMouseX = mx;
        mLastMouseY = my;
        mMouseInitialized = true;

        const bool mousePressed =
            Input::IsMouseDown(mts::MouseButton::Left) ||
            Input::IsMouseDown(mts::MouseButton::Right) ||
            Input::IsMouseDown(mts::MouseButton::Middle);

        bool anyKey = false;
        // Scan the engine keys that actually map to gameplay / menu input.
        // A handful of exemplars is enough — if any of these is down the
        // player is clearly on keyboard.
        const mts::Key watched[] = {
            mts::Key::W, mts::Key::A, mts::Key::S, mts::Key::D,
            mts::Key::Up, mts::Key::Down, mts::Key::Left, mts::Key::Right,
            mts::Key::Space, mts::Key::Enter, mts::Key::Escape,
            mts::Key::LeftShift, mts::Key::RightShift,
            mts::Key::J, mts::Key::K,
        };
        for (mts::Key k : watched)
        {
            if (Input::IsKeyDown(k))
            {
                anyKey = true;
                break;
            }
        }

        if (anyKey || mousePressed || mouseMoved)
            detected = InputDevice::Keyboard;
    }

    if (detected != mActiveDevice)
    {
        mActiveDevice = detected;
        if (mInstructionsSprite)
        {
            mts::Texture* tex = (mActiveDevice == InputDevice::Controller)
                                    ? mControllerTex
                                    : mKeyboardTex;
            if (tex)
                mInstructionsSprite->SetTexture(tex);
        }
    }
}

void PauseMenuPanel::SetVisible(bool visible)
{
    for (auto* go : mMembers)
    {
        if (go)
            go->SetActive(visible);
    }

    if (mNavigator)
    {
        if (visible)
            mNavigator->OnPanelShown();
        else
            mNavigator->OnPanelHidden();
    }
}

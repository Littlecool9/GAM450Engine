/**
 * @file PauseMenuPanel.cpp
 * @brief Visibility controller for the pause menu.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PauseMenuPanel.h"

#include "PauseMenuNavigator.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace
{
    // Names of the sibling GameObjects that make up the pause menu. Kept
    // in sync with CurrentScene.scene.
    constexpr const char* kMemberNames[] = {
        "PauseMenu_Background",
        "PauseMenu_Instructions",
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
}

void PauseMenuPanel::Start()
{
    // Hide the menu once all scripts have Awake'd/OnEnable'd and wired up
    // their subscriptions. Hiding earlier would skip those subscriptions.
    SetVisible(false);
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

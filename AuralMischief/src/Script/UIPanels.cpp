/**
 * @file UIPanels.h
 * @author Jackey Cen
 * @brief  Script that shows/hides a group of UI panel GameObjects; shared pause/unpause path for
 * ESC key and pause-icon button.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */
#include "pch.h"
#include "UIPanels.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/EntitySystem/Component/Button.h"

#include "ReturnButton.h"

void UIPanels::Awake()
{
    mMembers.clear();
    auto* scene = mts::SceneSystem::GetCurrentScene();
    if (!scene)
        return;
    for (const auto& uuid : kMemberUUIDs)
    {
        auto* go = scene->FindGameObjectByUUID(uuid);
        if (go)
        {
            mMembers.push_back(go);
        }
        else
            LOG_WARN("InstructionUI: member '{}' not found in scene", uuid.ToString());
    }
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("InstructionUI: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
        return;
    }
    mClickId = mButton->mOnClick.Subscribe([this]() { SetPanelActive(!mPanelActive); });
}

void UIPanels::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("UIObjects") && fields["UIObjects"].IsArray())
    {
        const auto& uiObjectsArray = fields["UIObjects"];
        size_t uiObjectCount = uiObjectsArray.Size();
        mMembers.clear();
        auto* scene = mts::SceneSystem::GetCurrentScene();
        if (!scene)
            return;
        for (size_t i = 0; i < uiObjectCount; ++i)
        {
            const auto& uiObjectJson = uiObjectsArray[i];
            kMemberUUIDs.emplace_back(mts::UUID::FromString(uiObjectJson.GetString()));
        }
    }
}

void UIPanels::Start()
{
    SetPanelActive(false);
}

void UIPanels::SetPanelActive(bool active)
{
    for (auto* go : mMembers)
    {
        go->SetActive(active);
        if (auto* returnButton = go->GetScript<ReturnButton>(); returnButton && active)
        {
            returnButton->SetMenuPanel(this);
            for (auto* btn : returnButton->mMainMenuButtons)
            {
                if (btn)
                    btn->SetEnabled(false);
            }
        }
    }
    mPanelActive = active;
}

void UIPanels::OnDisable()
{
    if (mButton)
        mButton->mOnClick.Unsubscribe(mClickId);
}

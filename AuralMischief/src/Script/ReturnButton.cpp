/**
 * @file ReturnButton.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ReturnButton.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

#include "UIPanels.h"

void ReturnButton::Awake()
{
	mButton = GetGameObject()->GetComponent<mts::Button>();
	if (!mButton)
	{
		LOG_WARN("ReturnButton: no Button component found on '{}'",
				 GetGameObject()->GetName().c_str());
	}
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

void ReturnButton::OnEnable()
{
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
}

void ReturnButton::OnDisable()
{
	if (mButton)
		mButton->mOnClick.Unsubscribe(mClickId);
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
}

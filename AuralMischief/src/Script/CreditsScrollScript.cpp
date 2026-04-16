/**
 * @file CreditsScrollScript.cpp
 * @author josh.siegel
 * @brief  credits scroll script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CreditsScrollScript.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/SceneSystem.h"

void CreditsScrollScript::Init()
{
    mActionList = GetGameObject()->GetComponent<mts::ActionList>();
}

void CreditsScrollScript::Awake()
{
    if (mActionList)
    {
        mActionList->AddTranslateAction(glm::vec3(0, 13, 0), GetGameObject(), 45);
        mActionList->SetFastForwardSpeed(4.0f);
    }
}

void CreditsScrollScript::OnUpdate(float dt)
{
    if (mActionList->IsEmpty() || mts::Input::IsKeyPressed(mts::Key::Escape))
        mts::SceneSystem::SwitchScene(1);
    if (mts::Input::IsKeyDown(mts::Key::Enter) || mts::Input::IsKeyDown(mts::Key::Space))
    {
        mActionList->FastForward();
    }
    else
    {
        mActionList->ResetSpeed();
    }
}

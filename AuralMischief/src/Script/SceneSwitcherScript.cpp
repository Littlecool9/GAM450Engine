/**
 * @file SceneSwitcherScript.cpp
 * @author sumin.park
 * @brief  switches scene on button click
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "SceneSwitcherScript.h"

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/SceneSystem.h"

void SceneSwitcherScript::Init()
{
    mButton = GetGameObject()->GetComponent<mts::Button>();
    if (!mButton)
    {
        LOG_WARN("SceneSwitcherScript: no Button component found on '{}'",
                 GetGameObject()->GetName().c_str());
    }
}

void SceneSwitcherScript::Awake()
{
    if (mButton)
    {
        mClickId =
            mButton->mOnClick.Subscribe([this]() { mts::SceneSystem::SwitchScene(mSceneIndex); });
    }
}

void SceneSwitcherScript::OnDisable()
{
    if (mButton)
    {
        mButton->mOnClick.Unsubscribe(mClickId);
    }
}

void SceneSwitcherScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("SceneIndex"))
        mSceneIndex = fields["SceneIndex"].GetInt(mSceneIndex);
}

void SceneSwitcherScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteInt("SceneIndex", mSceneIndex);
}

void SceneSwitcherScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "SceneIndex",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mSceneIndex},
                      .step = 1});
}

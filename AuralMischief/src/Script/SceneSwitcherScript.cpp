/**
 * @file SceneSwitcherScript.cpp
 * @author sumin.park
 * @brief  Switches to a registered scene when the corresponding digit key is pressed
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SceneSwitcherScript.h"

#include "Engine/Input/Input.h"
#include "Engine/SceneSystem/SceneSystem.h"

void SceneSwitcherScript::OnUpdate(float /*dt*/)
{
    static constexpr mts::Key digitKeys[] = {
        mts::Key::D0,
        mts::Key::D1,
        mts::Key::D2,
        mts::Key::D3,
        mts::Key::D4,
        mts::Key::D5,
        mts::Key::D6,
        mts::Key::D7,
        mts::Key::D8,
        mts::Key::D9,
    };

    for (int i = 0; i < 10; ++i)
    {
        if (mts::Input::IsKeyPressed(digitKeys[i]))
        {
            mts::SceneSystem::SwitchScene(i);
            return;
        }
    }
}

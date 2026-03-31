/**
 * @file SceneSwitcherScript.h
 * @author sumin.park
 * @brief  Switches to a registered scene when the corresponding digit key is pressed
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class SceneSwitcherScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(SceneSwitcherScript)

    void OnUpdate(float dt) override;
};

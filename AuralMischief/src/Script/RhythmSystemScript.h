/**
 * @file RhythmSystemScript.h
 * @author dream
 * @brief  Script that owns the RhythmSystem lifecycle (Init on Awake, Shutdown on OnDisable).
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class RhythmSystemScript : public mts::ScriptBase
{
   public:
    void Awake() override;
    void OnUpdate(float dt) override;
    void OnDisable() override;

    SCRIPT_CLASS(RhythmSystemScript)
};

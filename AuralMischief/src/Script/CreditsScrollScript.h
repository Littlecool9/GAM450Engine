/**
 * @file CreditsScrollScript.h
 * @author josh.siegel
 * @brief
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/Scripting/ScriptBase.h"


class CreditsScrollScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(CreditsScrollScript)

    void Init() override;
    void Awake() override;
    void OnUpdate(float dt) override;

   private:
    mts::ActionList* mActionList = nullptr;
};

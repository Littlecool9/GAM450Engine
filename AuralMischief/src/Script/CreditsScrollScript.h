/**
 * @file CreditsScrollScript.h
 * @author josh.siegel
 * @brief  credits scroll script
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
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

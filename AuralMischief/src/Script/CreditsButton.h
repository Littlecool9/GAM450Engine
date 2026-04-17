/**
 * @file CreditsButton.h
 * @author Jackey Cen
 * @brief  Script that switches to the gameplay scene when the main menu button is clicked.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Button;
}

class CreditsButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(CreditsButton)

    void Init() override;
    void Awake() override;
    void OnDisable() override;

   private:
    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
};

/**
 * @file ReturnToMainMenuButton.h
 * @author Jackey Cen
 * @brief Button script for the pause menu's "Return to Main Menu" entry.
 *        Restores time scale and audio before switching scenes, so the main
 *        menu isn't loaded frozen when the player returns while paused.
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

class PauseMenuNavigator;

class ReturnToMainMenuButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(ReturnToMainMenuButton)

    void Init() override;
    void OnEnable() override;
    void OnDisable() override;

   private:
    mts::Button* mButton{nullptr};
    PauseMenuNavigator* mNavigator{nullptr};
    mts::Delegate<>::Id mClickId{0};
};

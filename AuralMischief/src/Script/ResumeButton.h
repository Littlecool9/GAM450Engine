/**
 * @file ResumeButton.h
 * @brief Button script on the pause menu's "Resume" entry. Forwards the click
 *        to the scene's PauseButton::TogglePause() so pause/unpause and
 *        menu visibility stay in sync with the ESC shortcut and the header
 *        pause icon.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Button;
}

class PauseButton;

class ResumeButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(ResumeButton)

    void Init() override;
    void OnEnable() override;
    void OnDisable() override;

   private:
    mts::Button* mButton{nullptr};
    PauseButton* mPauseButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
};

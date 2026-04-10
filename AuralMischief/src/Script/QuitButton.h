/**
 * @file MainMenuButton.h
 * @brief  Script that switches to the gameplay scene when the main menu button is clicked.
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

class QuitButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(QuitButton)

    void Init() override;
    void Awake() override;
    void OnDisable() override;

   private:
    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
};

/**
 * @file QuitButton.h
 * @brief  Main-menu Quit button. Shows a confirmation dialog before quitting.
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
    class GameObject;
}

class QuitButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(QuitButton)

    void Init() override;
    void OnEnable() override;
    void OnDisable() override;

   private:
    void ShowConfirm();
    void HideConfirm();

    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};

    mts::GameObject* mConfirmBg{nullptr};
    mts::GameObject* mConfirmPrompt{nullptr};
    mts::GameObject* mConfirmYes{nullptr};
    mts::GameObject* mConfirmNo{nullptr};
    mts::Button* mYesButton{nullptr};
    mts::Button* mNoButton{nullptr};
    mts::Delegate<>::Id mYesClickId{0};
    mts::Delegate<>::Id mNoClickId{0};
};

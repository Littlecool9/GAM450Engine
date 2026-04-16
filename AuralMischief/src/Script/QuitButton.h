/**
 * @file QuitButton.h
 * @author Jackey Cen
 * @brief  Main-menu Quit button. Shows a confirmation dialog before quitting.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <vector>
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    class Button;
    class GameObject;
}  // namespace mts

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
    mts::GameObject* mConfirmNavigator{nullptr};
    mts::Button* mYesButton{nullptr};
    mts::Button* mNoButton{nullptr};
    mts::Delegate<>::Id mYesClickId{0};
    mts::Delegate<>::Id mNoClickId{0};

    // Sibling main-menu buttons that must be suppressed while the confirm
    // dialog is open. Their hit areas overlap the Yes/No buttons, so leaving
    // them enabled causes the underlying button to also fire on the same
    // mouse release (e.g. "No" click triggering HowToButton).
    std::vector<mts::Button*> mSiblingButtons;
};

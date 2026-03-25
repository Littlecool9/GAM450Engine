/**
 * @file ButtonTest.h
 * @author Dong Ta
 * @brief  Test script — logs to console when the Button on this GameObject is clicked.
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

class ButtonTest : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(ButtonTest)

    void OnCreate() override;
    void OnDisable() override;

   private:
    mts::Button* mButton{nullptr};
    mts::Delegate<>::Id mClickId{0};
    mts::Delegate<>::Id mHoverEnterId{0};
    mts::Delegate<>::Id mHoverExitId{0};
};

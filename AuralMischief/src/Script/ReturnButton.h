/**
 * @file ReturnButton.h
 * @author 11210
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include <vector>

namespace mts
{
    class Button;
    class JsonValue;
}

class UIPanels;

class ReturnButton : public mts::ScriptBase
{
    public:
    SCRIPT_CLASS(ReturnButton)
    void OnEnable() override;
    void OnDisable() override;
    void DeserializeFields(mts::JsonValue const& fields) override;

    void SetMenuPanel(UIPanels* panel) { mMenuPanel = panel; }
    std::vector<mts::Button*> mMainMenuButtons;

    private:
     mts::Button* mButton{nullptr};
     UIPanels* mMenuPanel{nullptr};
     mts::Delegate<>::Id mClickId{0};
     std::vector<mts::UUID> kMainMenuButtonUUIDs;
};

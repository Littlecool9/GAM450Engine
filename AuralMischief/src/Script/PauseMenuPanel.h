/**
 * @file PauseMenuPanel.h
 * @brief Visibility controller for the pause menu. Resolves the menu's member
 *        GameObjects (background, instructions, volume buttons, resume,
 *        return-to-main-menu) by name at scene load and toggles their active
 *        state on demand. Starts hidden so the menu only appears after the
 *        first pause.
 *
 *        Sibling GameObjects are used rather than a parent/child hierarchy
 *        because Mitosis scene JSON stores GameObjects as a flat list.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <vector>

#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class GameObject;
}

class PauseMenuNavigator;

class PauseMenuPanel : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(PauseMenuPanel)

    void Awake() override;
    void Start() override;

    // Show or hide every resolved menu member GameObject.
    void SetVisible(bool visible);

   private:
    std::vector<mts::GameObject*> mMembers;
    PauseMenuNavigator* mNavigator{nullptr};
};

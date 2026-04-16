/**
 * @file ReturnButton.h
 * @author Jackey Cen
 * @brief   Script for a button that returns to a parent menu panel when clicked; also listens for
 * ESC key to trigger click. Used for the back button on the pause menu and the instruction screen.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include <vector>
#include <glm/glm.hpp>

namespace mts
{
    class Button;
    class SpriteRenderer;
    class JsonValue;
}  // namespace mts

class UIPanels;

class ReturnButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(ReturnButton)
    void OnEnable() override;
    void OnUpdate(float dt) override;
    void OnDisable() override;
    void DeserializeFields(mts::JsonValue const& fields) override;

    void SetMenuPanel(UIPanels* panel) { mMenuPanel = panel; }
    std::vector<mts::Button*> mMainMenuButtons;

   private:
    mts::Button* mButton{nullptr};
    mts::SpriteRenderer* mSprite{nullptr};
    UIPanels* mMenuPanel{nullptr};
    mts::Delegate<>::Id mClickId{0};
    mts::Delegate<>::Id mHoverEnterId{0};
    mts::Delegate<>::Id mHoverExitId{0};
    std::vector<mts::UUID> kMainMenuButtonUUIDs;

    // Hover highlight — distinct from MenuNavigator's default so the back
    // button reads visually differently from the main menu items.
    glm::vec3 mHoverTint{0.6f, 1.4f, 1.4f};
    glm::vec3 mOriginalTint{1.0f};
    bool mHoverApplied{false};
};

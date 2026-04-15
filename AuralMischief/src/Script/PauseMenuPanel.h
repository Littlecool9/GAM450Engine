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
    class SpriteRenderer;
    class Texture;
}

class PauseMenuNavigator;

class PauseMenuPanel : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(PauseMenuPanel)

    void Awake() override;
    void Start() override;
    void OnUpdate(float dt) override;

    // Show or hide every resolved menu member GameObject.
    void SetVisible(bool visible);

   private:
    enum class InputDevice
    {
        Keyboard,
        Controller,
    };

    std::vector<mts::GameObject*> mMembers;
    PauseMenuNavigator* mNavigator{nullptr};

    // Cached sprite + textures for the keyboard/controller instruction swap.
    mts::SpriteRenderer* mInstructionsSprite{nullptr};
    mts::Texture* mKeyboardTex{nullptr};
    mts::Texture* mControllerTex{nullptr};

    InputDevice mActiveDevice{InputDevice::Keyboard};
    float mLastMouseX{0.0f};
    float mLastMouseY{0.0f};
    bool mMouseInitialized{false};
};

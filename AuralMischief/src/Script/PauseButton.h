/**
 * @file ButtonTest.h
 * @author Dong Ta
 * @brief  Test script — logs to console when the Button on this GameObject is clicked.
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
    class AudioManager;
}

class PauseMenuPanel;

class PauseButton : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(PauseButton)

    void Awake() override;
    void OnUpdate(float dt) override;
    void OnDisable() override;

    // Public so other UI scripts (e.g. the Resume button on the pause menu)
    // can route through the same pause/unpause code path as ESC and the
    // header pause icon.
    void TogglePause();

   private:
    mts::AudioManager* mAudio{nullptr};
    mts::Button* mButton{nullptr};
    PauseMenuPanel* mMenuPanel{nullptr};
    mts::Delegate<>::Id mClickId{0};
    mts::Delegate<>::Id mHoverEnterId{0};
    mts::Delegate<>::Id mHoverExitId{0};
    bool mPausedByThis{false};
    float mStoredTimeScale{1.0f};
};

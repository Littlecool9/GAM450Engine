/**
 * @file BossRoomTrigger.h
 * @author sumin.park
 * @brief  Tracks whether the player is currently inside the boss room trigger zone
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class BossRoomTrigger : public mts::ScriptBase
{
   public:
    void OnDisable() override;
    void OnCollisionExit2D(mts::Collision2D const& collision) override;

    bool IsPlayerInside() const { return mPlayerInside; }

    SCRIPT_CLASS(BossRoomTrigger)

   private:
    bool mPlayerInside = false;
};

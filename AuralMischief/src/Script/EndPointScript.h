/**
 * @file EndPointScript.h
 * @author sumin.park
 * @brief  trigger win
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class EndPointScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(EndPointScript)
    void OnCollisionEnter2D(const mts::Collision2D& collision) override;

   private:
};
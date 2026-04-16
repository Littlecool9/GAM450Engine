/**
 * @file HazardCollider.h
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

class HazardCollider : public mts::ScriptBase
{
   public:
    void OnCollisionEnter2D(const mts::Collision2D& collision) override;
    SCRIPT_CLASS(HazardCollider);
   private:
};

/**
 * @file CheckPoint.h
 * @author Jackey Cen
 * @brief: CheckPoint script for player respawn logic. When the player collides with this
 * checkpoint,
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class Transform;
}

class CheckPoint : public mts::ScriptBase
{
   public:
    void OnCollisionEnter2D(mts::Collision2D const& collision) override;

    glm::vec3 GetCheckPointPosition() const;

    SCRIPT_CLASS(CheckPoint)
   private:
};

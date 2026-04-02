/**
 * @file FadeInScript.h
 * @brief  Fades out the SpriteRenderer alpha from 1 to 0 over a configurable duration,
 *         creating a fade-in effect for the scene underneath.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"

namespace mts
{
    class SpriteRenderer;
}

class FadeInScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(FadeInScript)

    void Init() override;
    void Start() override;
    void OnUpdate(float dt) override;

   private:
    mts::SpriteRenderer* mRenderer{nullptr};
    float mDuration{1.0f};
    float mElapsed{0.0f};
};

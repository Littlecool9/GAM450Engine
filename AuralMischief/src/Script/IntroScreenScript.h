/**
 * @file IntroScreenScript.h
 * @author rahul
 * @brief Plays a fullscreen image sequence (fade-in / hold / fade-out) at scene start,
 *        mirroring the SplashScreen behaviour but driven from within a scene.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include <vector>

namespace mts
{
    class Texture;
    class Mesh;
    class Shader;
}  // namespace mts

class IntroScreenScript : public mts::ScriptBase
{
   public:
    SCRIPT_CLASS(IntroScreenScript)

    void Start() override;
    void OnUpdate(float dt) override;
    void OnRender() override;

   private:
    struct IntroEntry
    {
        mts::Texture* texture = nullptr;
        float fadeInDuration = 1.0f;
        float holdDuration = 2.0f;
        float fadeOutDuration = 1.0f;
    };

    std::vector<IntroEntry> mEntries;
    mts::Mesh* mMesh = nullptr;
    mts::Shader* mShader = nullptr;

    int mCurrentIndex = 0;
    float mElapsed = 0.0f;
    float mCurrentAlpha = 0.0f;
    bool mDone = false;
};

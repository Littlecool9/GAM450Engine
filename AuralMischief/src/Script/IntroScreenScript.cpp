/**
 * @file IntroScreenScript.cpp
 * @author rahul
 * @brief Plays a fullscreen image sequence (fade-in / hold / fade-out) at scene start,
 *        mirroring the SplashScreen behaviour but driven from within a scene.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "IntroScreenScript.h"

#include "Engine/Input/Input.h"
#include "Engine/Renderer.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include <glm/gtc/matrix_transform.hpp>

static const char* kTexPaths[] = {
    "Data/Resources/IntroArt01.tex",
    "Data/Resources/IntroArt02.tex",
    "Data/Resources/IntroArt03.tex",
    "Data/Resources/IntroArt04.tex",
};

void IntroScreenScript::Start()
{
    mMesh = mts::ResourceManager::GetMesh("Engine/Resources/DefaultMeshResource.mesh");
    mShader = mts::ResourceManager::GetShader("Engine/Resources/DefaultShaderResource.shader");

    if (!mMesh || !mShader)
    {
        LOG_WARN("IntroScreenScript: failed to load default mesh/shader — skipping intro.");
        mDone = true;
        return;
    }

    for (const char* path : kTexPaths)
    {
        mts::Texture* tex = mts::ResourceManager::GetTexture(path);
        if (tex)
            mEntries.push_back({tex, 1.0f, 2.0f, 1.0f});
        else
            LOG_WARN("IntroScreenScript: failed to load texture '{}' — skipping entry.", path);
    }

    mCurrentIndex = 0;
    mElapsed = 0.0f;
    mCurrentAlpha = 0.0f;
    mDone = mEntries.empty();
}

void IntroScreenScript::OnUpdate(float dt)
{
    if (mDone || mEntries.empty())
        return;

    // Advance to next image on any key/mouse press
    if (mts::Input::IsKeyPressed(mts::Key::Space) || mts::Input::IsKeyPressed(mts::Key::Escape) ||
        mts::Input::IsKeyPressed(mts::Key::Enter) ||
        mts::Input::IsMousePressed(mts::MouseButton::Left) ||
        mts::Input::IsMousePressed(mts::MouseButton::Right))
    {
        mElapsed = 0.0f;
        ++mCurrentIndex;
        if (mCurrentIndex >= static_cast<int>(mEntries.size()))
        {
            mDone = true;
            mts::SceneSystem::SwitchScene(0);
            return;
        }
    }

    mElapsed += dt;

    const IntroEntry& entry = mEntries[mCurrentIndex];
    const float totalTime = entry.fadeInDuration + entry.holdDuration + entry.fadeOutDuration;

    if (mElapsed >= totalTime)
    {
        mElapsed -= totalTime;
        ++mCurrentIndex;
        if (mCurrentIndex >= static_cast<int>(mEntries.size()))
        {
            mDone = true;
            mts::SceneSystem::SwitchScene(0);
            return;
        }
    }

    // Compute and cache alpha for OnRender
    const IntroEntry& cur = mEntries[mCurrentIndex];
    float alpha = 1.0f;
    if (mElapsed < cur.fadeInDuration)
        alpha = mElapsed / cur.fadeInDuration;
    else if (mElapsed >= cur.fadeInDuration + cur.holdDuration)
        alpha = 1.0f - (mElapsed - cur.fadeInDuration - cur.holdDuration) / cur.fadeOutDuration;
    mCurrentAlpha = (alpha < 0.0f) ? 0.0f : (alpha > 1.0f) ? 1.0f : alpha;
}

void IntroScreenScript::OnRender()
{
    if (mDone || mEntries.empty() || !mMesh || !mShader)
        return;

    // Full-screen model matrix (screen-space units, matching SplashScreen)
    glm::vec4 vp = mts::Renderer::GetViewportInfo();
    float w = vp.z / mts::PixelsPerUnit;
    float h = vp.w / mts::PixelsPerUnit;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(w * 0.5f, h * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    mts::RenderCommand cmd;
    cmd.shader = mShader;
    cmd.texture = mEntries[mCurrentIndex].texture;
    cmd.mesh = mMesh;
    cmd.model = model;
    cmd.tintColor = {1.0f, 1.0f, 1.0f};
    cmd.alpha = mCurrentAlpha;
    cmd.hasAlpha = true;
    cmd.isUI = true;
    cmd.sortKey = 9999.0f;

    mts::Renderer::Submit(cmd);
}

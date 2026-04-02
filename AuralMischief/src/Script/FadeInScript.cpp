/**
 * @file FadeInScript.cpp
 * @brief  Fades out the SpriteRenderer alpha from 1 to 0 over a configurable duration,
 *         creating a fade-in effect for the scene underneath.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "FadeInScript.h"

#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

void FadeInScript::Init()
{
    mRenderer = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    if (!mRenderer)
    {
        LOG_WARN("FadeInScript: no SpriteRenderer found on '{}'",
                 GetGameObject()->GetName().c_str());
    }
}

void FadeInScript::Start()
{
    mElapsed = 0.0f;
    if (mRenderer)
        mRenderer->SetAlpha(1.0f);
}

void FadeInScript::OnUpdate(float dt)
{
    if (!mRenderer)
        return;

    mElapsed += dt;
    float t = mElapsed / mDuration;

    if (t >= 1.0f)
    {
        mRenderer->SetAlpha(0.0f);
        GetGameObject()->SetActive(false);
        return;
    }

    mRenderer->SetAlpha(1.0f - t);
}

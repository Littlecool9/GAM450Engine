/**
 * @file CollectableController.cpp
 * @author josh.siegel
 * @brief // behavior script for collectable objects (ex. coins/gems)
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CollectableController.h"

#include "Engine/EntitySystem/Component/Audio/AudioEmitter.h"

void CollectableController::OnCreate()
{
    emitter = GetGameObject()->GetComponent<mts::AudioEmitter>();
}

void CollectableController::OnUpdate(float dt)
{
    if (collected && !emitter->IsPlaying())
    {
        GetGameObject()->Destroy();
    }
}

void CollectableController::OnCollisionEnter2D(const mts::GameObject* gameObject)
{
    if (gameObject->HasTag("Player"))
    {
        emitter->Play();
        collected = true;
        mts::SpriteRenderer* sr = GetGameObject()->GetComponent<mts::SpriteRenderer>();
        sr->SetAlpha(0.0f);
    }
}
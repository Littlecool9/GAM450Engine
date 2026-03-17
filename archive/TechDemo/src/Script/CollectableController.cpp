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

#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
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

void CollectableController::OnCollisionEnter2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->HasTag("Player"))
    {
        emitter->Play();
        collected = true;
        mts::SpriteRenderer* sr = GetGameObject()->GetComponent<mts::SpriteRenderer>();
        sr->SetAlpha(0.0f);
    }
}
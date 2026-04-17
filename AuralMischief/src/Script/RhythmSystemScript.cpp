/**
 * @file RhythmSystemScript.cpp
 * @author dream
 * @brief  Script that owns the RhythmSystem lifecycle (Init on Awake, Shutdown on OnDisable).
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "RhythmSystemScript.h"
#include "RhythmSystem.h"
#include "Engine/SceneSystem/SceneSystem.h"

void RhythmSystemScript::Awake()
{
    mts::Scene* scene = mts::SceneSystem::GetCurrentScene();
    RhythmSystem::Get().Init(scene);
}

void RhythmSystemScript::OnUpdate(float dt)
{
    RhythmSystem::Get().Update();
}

void RhythmSystemScript::OnDisable()
{
    RhythmSystem::Get().Shutdown();
}

/**
 * @file RhythmSystem.cpp
 * @author Jackey Cen
 * @brief RhythmSystem Manages rhythm-based gameplay logic, connecting ConductorSystem beats to
 * platform puzzles and other mechanics.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */

#include "pch.h"
#include "RhythmSystem.h"

#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "MovablePlatform.h"
#include "PlatformPuzzleScript.h"


MovablePlatform* GetMovablePlatformByUUID(mts::Scene* scene, const mts::UUID& uuid)
{
    auto* go = scene->FindGameObjectByUUID(uuid);
    if (!go)
        return nullptr;
    return go->GetScript<MovablePlatform>();
}

RhythmSystem* RhythmSystem::sInstance = nullptr;

RhythmSystem& RhythmSystem::Get()
{
    if (!sInstance)
    {
        static RhythmSystem instance;
        sInstance = &instance;
    }
    return *sInstance;
}

void RhythmSystem::Init(mts::Scene* scene)
{
    mScene = scene;
    mConductor = &mts::ConductorSystem::Get();

    if (mConductor)
    {
        mBeatSubId = mConductor->onBeat.Subscribe([this](mts::BeatInfo info) { OnBeat(info); });
        mBarSubId = mConductor->onBar.Subscribe([this](mts::BeatInfo info) { OnBar(info); });
    }

    for (auto& [puzzleId, state] : mPlatformPuzzles)
    {
        if (state.isActive)
            ActivatePlatformPuzzle(puzzleId);
    }
    LOG_INFO("[RhythmSystem] Initialized.");
}

void RhythmSystem::Shutdown()
{
    if (mConductor)
    {
        mConductor->onBeat.Unsubscribe(mBeatSubId);
        mConductor->onBar.Unsubscribe(mBarSubId);
    }

    mPlatformPuzzles.clear();
    mPlayerController = nullptr;
    mScene = nullptr;
    mConductor = nullptr;

    LOG_INFO("[RhythmSystem] Shutdown.");
}

void RhythmSystem::Update()
{

}

#pragma region Core OnBeat Delegate
void RhythmSystem::OnBeat(mts::BeatInfo info)
{
    // Platform logic along with the beats;
    for (auto& [puzzleId, state] : mPlatformPuzzles)
    {
        if (!state.isActive)
            continue;

        int defaultBeatInterval = state.config.beatInterval > 0 ? state.config.beatInterval : 4;
        int defaultHalfBeatInterval =
            state.config.halfBeatInterval > 0 ? state.config.halfBeatInterval : 0;

        int beatInterval = (state.config.beatInterval > 0) 
                               ? state.config.beatInterval 
                               : defaultBeatInterval;
        int halfBeatInterval = (state.config.halfBeatInterval > 0) 
                                   ? state.config.halfBeatInterval 
                                   : defaultHalfBeatInterval;

        if (!info.isHalfBeat)
        {
            if (halfBeatInterval > 0)
            {
                ++state.halfBeatCounter;
                if (state.halfBeatCounter >= halfBeatInterval)
                {
                    AdvancePlatformSequence(puzzleId);
                    state.halfBeatCounter = 0;
                }
            }
        }
        else
        {
            if (halfBeatInterval <= 0 && beatInterval > 0)
            {
                ++state.beatCounter;
                if (state.beatCounter >= beatInterval)
                {
                    AdvancePlatformSequence(puzzleId);
                    state.beatCounter = 0;
                }
            }
        }
    }
}

void RhythmSystem::OnBar(mts::BeatInfo info)
{

}
#pragma endregion

#pragma region External Functions
void RhythmSystem::RegisterPlatformPuzzle(const PlatformPuzzleConfig& config)
{
    if (config.puzzleId < 0)
    {
        LOG_ERROR("[RhythmSystem] Invalid puzzle ID: {}", config.puzzleId);
        return;
    }

    PlatformPuzzleState state;
    state.config = config;
    state.currentSequenceIndex = 0;
    state.beatCounter = 0;
    state.halfBeatCounter = 0;
    state.isActive = false;

    mPlatformPuzzles[config.puzzleId] = std::move(state);
    LOG_INFO("[RhythmSystem] Registered platform puzzle: {}", config.puzzleId);
}

void RhythmSystem::UnregisterPlatformPuzzle(int puzzleId)
{
    mPlatformPuzzles.erase(puzzleId);
}

void RhythmSystem::ActivatePlatformPuzzle(int puzzleId)
{
    auto it = mPlatformPuzzles.find(puzzleId);
    if (it != mPlatformPuzzles.end())
    {
        it->second.isActive = true;
        it->second.beatCounter = 0;
        it->second.halfBeatCounter = 0;
        LOG_INFO("[RhythmSystem] Activated platform puzzle: {}", puzzleId);
    }
}

void RhythmSystem::DeactivatePlatformPuzzle(int puzzleId)
{
    auto it = mPlatformPuzzles.find(puzzleId);
    if (it != mPlatformPuzzles.end())
    {
        it->second.isActive = false;
    }
}

void RhythmSystem::ResetPlatformPuzzle(int puzzleId)
{
    auto it = mPlatformPuzzles.find(puzzleId);
    if (it != mPlatformPuzzles.end())
    {
        it->second.currentSequenceIndex = 0;
        it->second.beatCounter = 0;
        it->second.halfBeatCounter = 0;

        auto& puzzleConfig = it->second.config;
        for (size_t i = 0; i < puzzleConfig.platformUuids.size(); ++i)
        {
            MovablePlatform* platformScript =
                GetMovablePlatformByUUID(mScene, puzzleConfig.platformUuids[i]);
            if (platformScript && !puzzleConfig.sequences[i].empty())
            {
                auto& entry = puzzleConfig.sequences[i][0];
                float speed = (entry.moveSpeed > 0.0f) ? entry.moveSpeed : 1.0f;    // default spped 1.0f
                platformScript->FirePlatformTranslate(entry.position, speed);
            }
        }
    }
}
#pragma endregion

void RhythmSystem::AdvancePlatformSequence(int puzzleId)
{
    auto it = mPlatformPuzzles.find(puzzleId);
    if (it == mPlatformPuzzles.end())
        return;

    auto& state = it->second;
    auto& config = state.config;

    int nextIndex = state.currentSequenceIndex + 1;

    // Check if reached the end first
    bool atEnd = false;
    for (size_t i = 0; i < config.sequences.size(); ++i)
    {
        if (nextIndex >= static_cast<int>(config.sequences[i].size()))
        {
            atEnd = true;
            break;
        }
    }

    if (atEnd)
    {
        if (config.loopSequence)
        {
            nextIndex = 0;
        }
        else
        {
            state.isActive = false;
            return;
        }
    }

    state.currentSequenceIndex = nextIndex;

    // Move all platforms
    for (size_t i = 0; i < config.platformUuids.size(); ++i)
    {
        MovablePlatform* platformScript = GetMovablePlatformByUUID(mScene, config.platformUuids[i]);
        if (!platformScript)
        {
            LOG_ERROR("[RhythmSystem] Could not find MovablePlatform with UUID {} for puzzle {}",
                      config.platformUuids[i].ToString(),
                      puzzleId);
            continue;
        }

        if (nextIndex < static_cast<int>(config.sequences[i].size()))
        {
            auto& entry = config.sequences[i][nextIndex];
            float speed = (entry.moveSpeed > 0.0f) ? entry.moveSpeed : 1.0f;
            platformScript->FirePlatformTranslate(entry.position, speed);
        }
    }

    onPlatformMoved.Invoke(puzzleId, nextIndex);
}

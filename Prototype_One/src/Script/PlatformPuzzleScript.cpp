/**
 * @file PlatformPuzzleScript.cpp
 * @author Jackey Cen
 * @brief See PlatformPuzzleScript.h
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */
#include "pch.h"
#include "PlatformPuzzleScript.h"

#include "MovablePlatform.h"
#include "RhythmConfigScript.h"
#include "RhythmSystem.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/Serialization/JsonAbstraction.h"
#include "Engine/UUID/UUID.h"

void PlatformPuzzleScript::OnCreate()
{
    // Generate puzzle id with GameObject UUID
    if (mPuzzleId < 0)
    {
        mPuzzleId = static_cast<int>(GetGameObject()->GetId().High() % 10000);
    }
}

void PlatformPuzzleScript::OnDestroy()
{
    UnregisterFromRhythmSystem();
}

void PlatformPuzzleScript::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("PuzzleId"))
        mPuzzleId = fields["PuzzleId"].GetInt(mPuzzleId);
    if (fields.Has("BeatInterval"))
        mBeatInterval = fields["BeatInterval"].GetInt(mBeatInterval);
    if (fields.Has("HalfBeatInterval"))
        mHalfBeatInterval = fields["HalfBeatInterval"].GetInt(mHalfBeatInterval);
    if (fields.Has("LoopSequence"))
        mLoopSequence = fields["LoopSequence"].GetBool(mLoopSequence);
    if (fields.Has("AutoActivate"))
        mAutoActivate = fields["AutoActivate"].GetBool(mAutoActivate);

    // --- Deserialize platform configurations ---
    if (fields.Has("Platforms") && fields["Platforms"].IsArray())
    {
        const auto& platformsArray = fields["Platforms"];
        size_t platformCount = platformsArray.Size();

        for (size_t i = 0; i < platformCount; ++i)
        {
            const auto& platformJson = platformsArray[i];
            if (!platformJson.IsObject())
            {
                LOG_WARN("[PlatformPuzzleScript] Platform entry {} in puzzle {} is not an object",
                         i,
                         mPuzzleId);
                continue;
            }

            PlatformBehaviorConfig entry;
            // Parse UUID (required)
            if (platformJson.Has("UUID"))
            {
                entry.platformUuid = mts::UUID::FromString(platformJson["UUID"].GetString());
            }
            else
            {
                LOG_WARN("[PlatformPuzzleScript] Platform entry {} missing 'UUID' in puzzle {}",
                         i,
                         mPuzzleId);
                continue;
            }

            // Parse Positions array (required)
            if (platformJson.Has("Positions") && platformJson["Positions"].IsArray())
            {
                const auto& positionsArray = platformJson["Positions"];

                for (size_t j = 0; j < positionsArray.Size(); ++j)
                {
                    const auto& positionJson = positionsArray[j];
                    if (!positionJson.IsObject())
                    {
                        LOG_WARN(
                            "[PlatformPuzzleScript] Position entry {} for platform {} in puzzle {} "
                            "is not an object",
                            j,
                            i,
                            mPuzzleId);
                        continue;
                    }

                    PlatformBehaviorEntry behaviorEntry;

                    // Parse Position [x, y, z]
                    if (positionJson.Has("Position") && positionJson["Position"].IsArray())
                    {
                        const auto& posArray = positionJson["Position"];
                        if (posArray.Size() >= 3)
                        {
                            behaviorEntry.position.x = posArray[0].GetFloat(0.0f);
                            behaviorEntry.position.y = posArray[1].GetFloat(0.0f);
                            behaviorEntry.position.z = posArray[2].GetFloat(0.0f);
                        }
                        else
                        {
                            LOG_WARN(
                                "[PlatformPuzzleScript] Position array for entry {} in platform {} "
                                "has insufficient elements (expected 3) in puzzle {}",
                                j,
                                i,
                                mPuzzleId);
                        }
                    }
                    else
                    {
                        LOG_WARN(
                            "[PlatformPuzzleScript] Position entry {} for platform {} missing "
                            "'Position' array in puzzle {}",
                            j,
                            i,
                            mPuzzleId);
                    }

                    // Parse Speed
                    if (positionJson.Has("Speed"))
                    {
                        behaviorEntry.moveSpeed = positionJson["Speed"].GetFloat(5.0f);
                    }
                    else
                    {
                        LOG_WARN(
                            "[PlatformPuzzleScript] Position entry {} for platform {} missing "
                            "'Speed' in puzzle {}",
                            j,
                            i,
                            mPuzzleId);
                    }

                    entry.behavior.emplace_back(std::move(behaviorEntry));
                }

            }
            else
            {
                LOG_WARN(
                    "[PlatformPuzzleScript] Platform entry {} missing 'Positions' array in puzzle "
                    "{}",
                    i,
                    mPuzzleId);
            }
            
            mPlatforms.emplace_back(std::move(entry));
        }
    }

    //ResolvePlatformReferences();
    if (mAutoActivate)
    {
        RegisterToRhythmSystem();
        Activate();
    }
    else
    {
        RegisterToRhythmSystem();
    }
}

void PlatformPuzzleScript::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteInt("PuzzleId", mPuzzleId);
    out.WriteInt("BeatInterval", mBeatInterval);
    out.WriteInt("HalfBeatInterval", mHalfBeatInterval);
    out.WriteBool("LoopSequence", mLoopSequence);
    out.WriteBool("AutoActivate", mAutoActivate);

    out.BeginArray("Platforms");
    for (const auto& platform : mPlatforms)
    {
        out.BeginObject();

        out.WriteBool("UUID",
                      platform.platformUuid.IsValid() ? platform.platformUuid.ToString().c_str() : "");

        out.BeginArray("Positions");
        for (const auto& behavior : platform.behavior)
        {
            out.BeginObject();
            out.WriteVec3("Position", behavior.position);
            out.WriteFloat("Speed", behavior.moveSpeed);
            out.EndObject();
        }
        out.EndArray();

        out.EndObject();
    }
    out.EndArray();
}

void PlatformPuzzleScript::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back(
        {.name = "Puzzle ID", .type = mts::ScriptField::Type::Int, .ptr = {.i = &mPuzzleId}});
    fields.push_back({.name = "Beat Interval",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mBeatInterval}});
    fields.push_back({.name = "HalfBeat Interval",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mHalfBeatInterval}});
    fields.push_back({.name = "Loop Sequence",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mLoopSequence}});
    fields.push_back({.name = "Auto Activate",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mAutoActivate}});

    // Recommand using JSON for configuring currently
}

void PlatformPuzzleScript::Activate()
{
    if (!mIsRegistered)
    {
        RegisterToRhythmSystem();
    }
    RhythmSystem::Get().ActivatePlatformPuzzle(mPuzzleId);
    LOG_INFO("[PlatformPuzzleScript] Puzzle {} activated", mPuzzleId);
}

void PlatformPuzzleScript::Deactivate()
{
    RhythmSystem::Get().DeactivatePlatformPuzzle(mPuzzleId);
    LOG_INFO("[PlatformPuzzleScript] Puzzle {} deactivated", mPuzzleId);
}

void PlatformPuzzleScript::Reset()
{
    RhythmSystem::Get().ResetPlatformPuzzle(mPuzzleId);
    LOG_INFO("[PlatformPuzzleScript] Puzzle {} reset", mPuzzleId);
}

void PlatformPuzzleScript::ResolvePlatformReferences()
{
    mts::Scene* scene = mts::SceneSystem::GetCurrentScene();
    if (!scene)
    {
        LOG_ERROR("[PlatformPuzzleScript] No active scene to resolve platform references");
        return;
    }

    for (auto& platform : mPlatforms)
    {
        if (!platform.platformUuid.IsValid())
        {
            LOG_WARN("[PlatformPuzzleScript] Invalid UUID in puzzle {}", mPuzzleId);
            continue;
        }
        mts::GameObject* go = scene->FindGameObjectByUUID(platform.platformUuid);
        if (!go)
        {
            LOG_WARN("[PlatformPuzzleScript] Could not find GameObject with UUID {} for puzzle {}",
                     platform.platformUuid.ToString(),
                     mPuzzleId);
            continue;
        }
        auto* platformScript = go->GetScript<MovablePlatform>();
        if (!platformScript)
        {
            LOG_WARN(
                "[PlatformPuzzleScript] GameObject '{}' with UUID {} does not have "
                "MovablePlatform script for puzzle {}",
                go->GetName(),
                platform.platformUuid.ToString(),
                mPuzzleId);
            continue;
        }
        platform.resolvedPlatform = platformScript;
    }

    LOG_INFO("[PlatformPuzzleScript] Resolved {} platform references for puzzle {}",
             mPlatforms.size(),
             mPuzzleId);
}

void PlatformPuzzleScript::RegisterToRhythmSystem()
{
    if (mIsRegistered)
        return;

    PlatformPuzzleConfig puzzleConfig;
    puzzleConfig.puzzleId = mPuzzleId;
    puzzleConfig.beatInterval = mBeatInterval;
    puzzleConfig.halfBeatInterval = mHalfBeatInterval;
    puzzleConfig.loopSequence = mLoopSequence;
    puzzleConfig.triggerZone = nullptr;

    for (const auto& platform : mPlatforms)
    {

        puzzleConfig.platformUuids.push_back(platform.platformUuid);

        std::vector<PlatformBehaviorEntry> sequence;
        for (const auto& platformInfo : platform.behavior)
        {
            sequence.push_back({platformInfo.position, platformInfo.moveSpeed});
        }
        puzzleConfig.sequences.push_back(std::move(sequence));
    }

    RhythmSystem::Get().RegisterPlatformPuzzle(puzzleConfig);
    mIsRegistered = true;

    LOG_INFO("[PlatformPuzzleScript] Registered puzzle {} with {} platforms",
             mPuzzleId,
             puzzleConfig.platformUuids.size());
}

void PlatformPuzzleScript::UnregisterFromRhythmSystem()
{
    if (!mIsRegistered)
        return;

    RhythmSystem::Get().UnregisterPlatformPuzzle(mPuzzleId);
    mIsRegistered = false;

    LOG_INFO("[PlatformPuzzleScript] Unregistered puzzle {}", mPuzzleId);
}

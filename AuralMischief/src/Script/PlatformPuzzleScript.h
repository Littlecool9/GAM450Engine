/**
 * @file PlatformPuzzleScript.h
 * @author Jackey Cen
 * @brief NativeScript for configuring a platform puzzle in JSON.
 *        Each puzzle entity has this script attached to configure its platforms,
 *        movement sequences, and timing via UUID references.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 */
#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/UUID/UUID.h"
#include "RhythmSystem.h"
#include <glm/vec3.hpp>
#include <vector>

class MovablePlatform;
struct PlatformBehaviorEntry;

struct PlatformBehaviorConfig
{
    mts::UUID platformUuid = mts::UUID::InvalidID;
    MovablePlatform* resolvedPlatform = nullptr;
    std::vector<PlatformBehaviorEntry> behavior;
};

/**
 * @brief PlatformPuzzleScript - Platform Puzzle Configuration Script
 *
 * JSON Example:
 * @code
 * {
 *     "Type": "NativeScript",
 *     "Data": {
 *         "Class": "PlatformPuzzleScript",
 *         "Fields": {
 *             "PuzzleId": 1,
 *             "BeatInterval": 4,
 *             "HalfBeatInterval": 0,
 *             "LoopSequence": true,
 *             "AutoActivate": true,
 *             "Platforms": 
                [
 *                  {
                        "UUID": "platform-1-uuid-here"
 *                      "Positions": [
 *                         { "Position": [0, 0, 0], "Speed": 5.0 },
 *                         { "Position": [5, 0, 0], "Speed": 5.0 },
 *                         { "Position": [5, 3, 0], "Speed": 5.0 }
 *                         { "Position": [10, 3, 0], "Speed": 5.0 },
 *                         { "Position": [5, 3, 0], "Speed": 5.0 },
 *                         { "Position": [5, 0, 0], "Speed": 5.0 }
 *                      ],
 *                  },
 *                  {
                        "UUID": "platform-2-uuid-here"
 *                      "Positions": [
 *                         { "Position": [0, 0, 0], "Speed": 5.0 },
 *                         { "Position": [5, 0, 0], "Speed": 5.0 },
 *                         { "Position": [5, 3, 0], "Speed": 5.0 }
 *                         { "Position": [10, 3, 0], "Speed": 5.0 },
 *                         { "Position": [5, 3, 0], "Speed": 5.0 },
 *                         { "Position": [5, 0, 0], "Speed": 5.0 }
 *                      ],
 *                  }
 *              ]       
 *          }
 *     }
 * }
 * @endcode
 */

class PlatformPuzzleScript : public mts::ScriptBase
{
   public:
    void Init() override;
    void Awake() override;
    void OnDisable() override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    SCRIPT_CLASS(PlatformPuzzleScript)

    // --- Getters ---
    int GetPuzzleId() const { return mPuzzleId; }
    int GetBeatInterval() const { return mBeatInterval; }
    int GetHalfBeatInterval() const { return mHalfBeatInterval; }
    bool IsLoopSequence() const { return mLoopSequence; }
    bool IsAutoActivate() const { return mAutoActivate; }

    // --- Control ---
    void Activate();
    void Deactivate();
    void Reset();

   private:
    // --- Puzzle Config ---
    int mPuzzleId = -1;
    int mBeatInterval = 0;      // Move every N beats (0 for no beat requirement)
    int mHalfBeatInterval = 0;  // Move every N half-beats (0 for no half-beat requirement)
    bool mLoopSequence = true;
    bool mAutoActivate = true;  // auto activated OnCreate
    bool mIsRegistered = false;

    std::vector<PlatformBehaviorConfig> mPlatforms;

    void ResolvePlatformReferences();
    void RegisterToRhythmSystem();
    void UnregisterFromRhythmSystem();

};

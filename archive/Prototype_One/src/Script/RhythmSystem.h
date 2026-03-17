/**
 * @file RhythmSystem.h
 * @author Jackey Cen
 * @brief: Connect ConductorySystem with gameplay logic.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */

#pragma once

#include <vector>
#include <unordered_map>
#include "Engine/Delegate/Delegate.h"
#include "Engine/Conductor/ConductorSystem.h"
#include <glm/vec3.hpp>

class PlayerController;
class MovablePlatform;
class CheckPoint;

namespace mts
{
    class GameObject;
    class Scene;
}  

struct PlatformBehaviorEntry
{
    glm::vec3 position{0.0f};
    float moveSpeed = 0.0f;
};

struct PlatformPuzzleConfig
{
    int puzzleId = -1;

    // The index of these two vectors should match, i.e. platforms[i] corresponds to sequences[i]
    //std::vector<MovablePlatform*> platforms;
    std::vector<mts::UUID> platformUuids;
    std::vector<std::vector<PlatformBehaviorEntry>> sequences;

    int beatInterval = 0;       
    int halfBeatInterval = 0;   
    bool loopSequence = true;   

    mts::GameObject* triggerZone = nullptr;
};

class RhythmSystem
{
   public:
    static RhythmSystem& Get();

    void Init(mts::Scene* scene);
    void Shutdown();
    void Update();

    void SetPlayerController(PlayerController* player) { mPlayerController = player; }
    PlayerController* GetPlayerController() const { return mPlayerController; }

    void RegisterPlatformPuzzle(const PlatformPuzzleConfig& config);
    void UnregisterPlatformPuzzle(int puzzleId);
    void ActivatePlatformPuzzle(int puzzleId);
    void DeactivatePlatformPuzzle(int puzzleId);
    void ResetPlatformPuzzle(int puzzleId); 

    mts::Delegate<int /*puzzleId*/, int /*sequenceIndex*/> onPlatformMoved;

   private:
    RhythmSystem() = default;
    ~RhythmSystem() = default;

    RhythmSystem(const RhythmSystem&) = delete;
    RhythmSystem& operator=(const RhythmSystem&) = delete;

    void OnBeat(mts::BeatInfo info);
    void OnBar(mts::BeatInfo info);
    void AdvancePlatformSequence(int puzzleId);

    mts::Scene* mScene = nullptr;
    mts::ConductorSystem* mConductor = nullptr;
    PlayerController* mPlayerController = nullptr;

    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;

    struct PlatformPuzzleState
    {
        PlatformPuzzleConfig config;
        int currentSequenceIndex = 0;
        int beatCounter = 0;      // Beat counter for beats since last move
        int halfBeatCounter = 0;  // Beat counter for half-beats since last move
        bool isActive = false;
    };
    std::unordered_map<int, PlatformPuzzleState> mPlatformPuzzles;

    static RhythmSystem* sInstance;

};
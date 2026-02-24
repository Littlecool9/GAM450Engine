/**
 * @file AudioManager.h
 * @author rahul
 * @brief Engine-facing audio service built on IAudioBackend
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <unordered_set>

#include "AudioTypes.h"
#include "IAudioBackend.h"

namespace mts
{
    // Separating into categories so that gameplay doesn't use bus paths directly
    enum class AudioCategory : uint8_t
    {
        Master,
        Music,
        SFX,
        UI,
        Ambience
    };

    // Can be expanded later to include AudioProject as an import option
    enum class BankGroup : uint8_t
    {
        Core,
        UI,
        Music,
        Level
    };

    // Rhythm track metadata (can be extended to be handled via import system)
    struct MusicTrack
    {
        std::string trackId;
        AudioEventId eventId;
        float bpm = 120.f;
        int beatsPerBar = 4;
        float offsetSec = 0.f;  // beat-0 alignment
    };

    class AudioManager
    {
       public:

        static AudioManager& Get() { return *sManager; }
        static void SetManager(AudioManager* mgr) { sManager = mgr; }



        AudioManager();
        ~AudioManager();

        // --- Lifecycle ---
        bool Init(std::unique_ptr<IAudioBackend> backend, const AudioConfig& config);
        void Shutdown();
        void Update(float dt);

        bool isInitialized() const { return mInitialized; }

        // --- Listener (2D) ---
        void SetListener2D(glm::vec2 pos, glm::vec2 vel = {0.f, 0.f});
        glm::vec2 GetListener2D() { return mListenerPos; }

        // --- Spatial instance helpers ---
        void SetInstance2D(AudioHandle h, glm::vec2 pos, glm::vec2 vel = {0.f, 0.f}) const;

        // --- Banks ---
        void AddBankToGroup(BankGroup group, const std::string& bankPath);
        bool LoadBankGroup(BankGroup group);
        bool UnloadBankGroup(BankGroup group);

        // --- Playback ---
        AudioHandle Play(const AudioEventId& eventId) const;                   // Non-spatial
        AudioHandle Play2D(const AudioEventId& eventId, glm::vec2 pos) const;  // Spatial 2D (Z=0)
        void Stop(AudioHandle h, StopMode mode) const;
        void Release(AudioHandle h) const;
        bool IsPlaying(AudioHandle h) const;
        // One-shot helpers
        void PlayOneShot(const AudioEventId& eventId) const;
        void PlayOneShot2D(const AudioEventId& eventId, glm::vec2 pos) const;

        // --- Parameters ---
        void SetParameter(AudioHandle h, const std::string& name, float value) const;
        void SetGlobalParameter(const std::string& name, float value) const;

        // --- Mixing ---
        void SetCategoryVolume(AudioCategory category, float volume01) const;
        void SetCategoryPaused(AudioCategory category, bool paused) const;

        // --- Snapshots (engine-level wrapper) ---
        AudioHandle StartSnapshot(const std::string& snapshotEventId) const;
        void StopSnapshot(AudioHandle snapshotHandle, StopMode mode = StopMode::AllowFadeOut) const;

        // --- Rhythm/Music ---
        // Too minimal right now needs to be revisited during prototyping
        bool StartMusic(const MusicTrack& track);
        void StopMusic(StopMode mode = StopMode::AllowFadeOut);

        float GetSongTimeSeconds() const { return mSongTime; }
        int GetBeatIndex() const { return mBeatIndex; }
        float GetBeatPhase01() const { return mBeatPhase; }
        bool IsInBeatWindow(float earlySec, float lateSec) const;
        int  GetBeatsPerBar() const { return mCurrentTrack.beatsPerBar; }
        bool IsMusicPlaying() const { return mMusicPlaying; }

       private:
        // Helper methods
        bool EnsureInitOrWarn(const char* fn) const;
        void UpdateRhythm(float dt);
        const char* GetBusPath(AudioCategory category) const;

        bool mInitialized = false;
        AudioConfig mConfig{};
        std::unique_ptr<IAudioBackend> mBackend;

        // Listener cache (2D)
        glm::vec2 mListenerPos{0.f, 0.f};
        glm::vec2 mListenerVel{0.f, 0.f};

        // Bank groups
        std::unordered_map<BankGroup, std::vector<std::string>> mBankGroups;
        std::unordered_set<BankGroup> mLoadedGroups;

        // Bus mapping (to make into convenient FMOD bus paths)
        std::unordered_map<AudioCategory, std::string> mBusPaths;

        // Rhythm state
        MusicTrack mCurrentTrack{};
        AudioHandle mMusicHandle{};
        bool mMusicPlaying = false;
        float mSongTime = 0.f;
        int mBeatIndex = 0;
        float mBeatPhase = 0.f;

        static AudioManager* sManager;
    };

}  // namespace mts
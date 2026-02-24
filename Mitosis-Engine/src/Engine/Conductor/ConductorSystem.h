/**
 * @file ConductorSystem.h
 * @author rahul
 * @brief Rhythm conductor: detects beat/half-beat/bar crossings from AudioManager
 *        and dispatches them to game-layer subscribers via Delegate<BeatInfo>.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/Delegate/Delegate.h"

namespace mts
{
    // Payload carried by every beat / half-beat delegate invocation.
    // A single delegate type covers both full beats and half-beat subdivisions;
    // subscribers can branch on isHalfBeat when they need to distinguish.
    struct BeatInfo
    {
        int   beatIndex;      // Absolute beat number since music start (0-based)
        int   halfBeatIndex;  // beatIndex * 2 + (0 for full beat, 1 for half-beat)
        int   barIndex;       // Bar (measure) number (0-based)
        int   beatInBar;      // Beat position within the bar (0 .. beatsPerBar-1)
        float beatPhase;      // 0..1 progress within the current beat
        bool  isHalfBeat;     // false = full beat, true = half-beat subdivision
    };

    class ConductorSystem
    {
    public:
        // --- Singleton access (mirrors AudioManager pattern) ---
        static ConductorSystem& Get() { return *sInstance; }
        static void SetInstance(ConductorSystem* inst) { sInstance = inst; }

        // --- Lifecycle (called by Application) ---
        void Init();
        void Update(float dt);  // Reads AudioManager state, fires delegates
        void Shutdown();

        // --- Query API ---
        // All values are 0 / false when no music is playing.
        int   GetBeatIndex()     const { return mBeatIndex; }
        int   GetHalfBeatIndex() const { return mHalfBeatIndex; }
        int   GetBarIndex()      const { return mBarIndex; }
        int   GetBeatInBar()     const { return mBeatInBar; }
        float GetBeatPhase()     const { return mBeatPhase; }
        bool  IsMusicPlaying()   const { return mMusicPlaying; }

        // --- Delegates ---
        // onBeat fires on every full beat AND every half-beat (check BeatInfo::isHalfBeat).
        // onBar  fires only on beat 0 of each new bar (measure downbeat).
        Delegate<BeatInfo> onBeat;
        Delegate<BeatInfo> onBar;

    private:
        void     ResetState();
        BeatInfo MakeBeatInfo(bool isHalfBeat) const;

        // Cached state derived from AudioManager each frame
        int   mBeatIndex     = 0;
        int   mHalfBeatIndex = 0;
        int   mBarIndex      = 0;
        int   mBeatInBar     = 0;
        float mBeatPhase     = 0.f;
        bool  mMusicPlaying  = false;

        // Previous-frame values used for crossing detection
        int   mPrevBeatIndex     = -1;
        int   mPrevHalfBeatIndex = -1;
        int   mPrevBarIndex      = -1;

        static ConductorSystem* sInstance;
    };

}  // namespace mts

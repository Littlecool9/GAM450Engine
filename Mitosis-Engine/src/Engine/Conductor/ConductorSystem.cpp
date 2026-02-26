/**
 * @file ConductorSystem.cpp
 * @author rahul
 * @brief See ConductorSystem.h
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ConductorSystem.h"
#include "Engine/Audio/AudioManager.h"

namespace mts
{
    ConductorSystem* ConductorSystem::sInstance = nullptr;

    // -------------------------------------------------------------------------
    // Lifecycle
    // -------------------------------------------------------------------------

    void ConductorSystem::Init()
    {
        ResetState();
        LOG_INFO("ConductorSystem initialized.");
    }

    void ConductorSystem::Shutdown()
    {
        onBeat.Clear();
        onBar.Clear();
        ResetState();
        LOG_INFO("ConductorSystem shut down.");
    }

    // -------------------------------------------------------------------------
    // Update — pull rhythm state from AudioManager, fire delegates on crossings
    // -------------------------------------------------------------------------

    void ConductorSystem::Update(float /*dt*/)
    {
        AudioManager& audio = AudioManager::Get();
        mMusicPlaying = audio.IsMusicPlaying();

        if (!mMusicPlaying)
        {
            ResetState();
            return;
        }

        const int newBeat = audio.GetBeatIndex();
        const float newPhase = audio.GetBeatPhase01();
        const int beatsPerBar = audio.GetBeatsPerBar();
        // Half-beat index: even = full beat position, odd = mid-beat subdivision
        const int newHalfBeat = newBeat * 2 + (newPhase >= 0.5f ? 1 : 0);

        mBeatPhase = newPhase;

        // --- Full beat crossing ---
        if (newBeat != mPrevBeatIndex)
        {
            mBeatIndex = newBeat;
            mBeatInBar = (beatsPerBar > 0) ? (mBeatIndex % beatsPerBar) : 0;
            mBarIndex = (beatsPerBar > 0) ? (mBeatIndex / beatsPerBar) : 0;
            mHalfBeatIndex = newBeat * 2;  // reset to even on a full beat

            onBeat.Invoke(MakeBeatInfo(false));

            if (mBeatInBar == 0 && mBarIndex != mPrevBarIndex)
            {
                onBar.Invoke(MakeBeatInfo(false));
                mPrevBarIndex = mBarIndex;
            }

            mPrevBeatIndex = mBeatIndex;
            mPrevHalfBeatIndex = mHalfBeatIndex;
        }

        // --- Half-beat crossing (odd indices only — skip the even one fired above) ---
        if (newHalfBeat != mPrevHalfBeatIndex && newHalfBeat % 2 == 1)
        {
            mHalfBeatIndex = newHalfBeat;
            onBeat.Invoke(MakeBeatInfo(true));
            mPrevHalfBeatIndex = mHalfBeatIndex;
        }
    }

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    BeatInfo ConductorSystem::MakeBeatInfo(bool isHalfBeat) const
    {
        return {mBeatIndex, mHalfBeatIndex, mBarIndex, mBeatInBar, mBeatPhase, isHalfBeat};
    }

    void ConductorSystem::ResetState()
    {
        mBeatIndex = mHalfBeatIndex = mBarIndex = mBeatInBar = 0;
        mBeatPhase = 0.f;
        mMusicPlaying = false;
        mPrevBeatIndex = mPrevHalfBeatIndex = mPrevBarIndex = -1;
    }

}  // namespace mts

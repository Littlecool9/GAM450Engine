#include "pch.h"
#include "Engine/Conductor/BeatMath.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/Audio/AudioManager.h"
#include "TestHelpers/EngineTestFixture.h"

// =============================================================================
// Group A -- Pure BeatMath tests (no fixture)
// =============================================================================

TEST(BeatMath_BeatFromTime, AtTimeZero)
{
    auto [beat, phase] = mts::BeatMath::BeatFromTime(0.f, 120.f, 0.f);
    EXPECT_EQ(beat, 0);
    EXPECT_FLOAT_EQ(phase, 0.f);
}

TEST(BeatMath_BeatFromTime, ExactBoundary_120BPM)
{
    // At 120 BPM, one beat = 0.5 s. t=0.5 should land exactly on beat 1.
    auto [beat, phase] = mts::BeatMath::BeatFromTime(0.5f, 120.f, 0.f);
    EXPECT_EQ(beat, 1);
    EXPECT_NEAR(phase, 0.f, 1e-5f);
}

TEST(BeatMath_BeatFromTime, MidBeat_120BPM)
{
    // At 120 BPM, 0.25 s = halfway through beat 0.
    auto [beat, phase] = mts::BeatMath::BeatFromTime(0.25f, 120.f, 0.f);
    EXPECT_EQ(beat, 0);
    EXPECT_NEAR(phase, 0.5f, 1e-5f);
}

TEST(BeatMath_BeatFromTime, WithPositiveOffset)
{
    // offset=0.5s; at songTime=0.5 we're at t=0, so {0, 0}.
    auto [beat, phase] = mts::BeatMath::BeatFromTime(0.5f, 120.f, 0.5f);
    EXPECT_EQ(beat, 0);
    EXPECT_NEAR(phase, 0.f, 1e-5f);

    // at songTime=1.0 we're at t=0.5 => beat 1 exactly
    auto [beat2, phase2] = mts::BeatMath::BeatFromTime(1.0f, 120.f, 0.5f);
    EXPECT_EQ(beat2, 1);
    EXPECT_NEAR(phase2, 0.f, 1e-5f);
}

TEST(BeatMath_BeatFromTime, PreRoll_Clamps)
{
    // songTimeSec < offsetSec: pre-roll, result should be {0, 0}.
    auto [beat, phase] = mts::BeatMath::BeatFromTime(0.1f, 120.f, 0.5f);
    EXPECT_EQ(beat, 0);
    EXPECT_FLOAT_EQ(phase, 0.f);
}

TEST(BeatMath_BeatFromTime, At140BPM)
{
    // At 140 BPM, beatDur = 60/140 ≈ 0.42857 s. After 1 full beat:
    const float beatDur = 60.f / 140.f;
    auto [beat, phase] = mts::BeatMath::BeatFromTime(beatDur, 140.f, 0.f);
    EXPECT_EQ(beat, 1);
    EXPECT_NEAR(phase, 0.f, 1e-4f);

    // Halfway through beat 0:
    auto [beat2, phase2] = mts::BeatMath::BeatFromTime(beatDur * 0.5f, 140.f, 0.f);
    EXPECT_EQ(beat2, 0);
    EXPECT_NEAR(phase2, 0.5f, 1e-4f);
}

TEST(BeatMath_BarPhase, ZeroPhase)
{
    float p = mts::BeatMath::BarPhase(0, 0.f, 4, 1.f);
    EXPECT_FLOAT_EQ(p, 0.f);
}

TEST(BeatMath_BarPhase, SpeedMultiplier2_Wraps)
{
    // beatInBar=2, beatPhase=0, 4/4 => base = 0.5; *2 = 1.0; fmod(1.0, 1.0) = 0.0
    float p = mts::BeatMath::BarPhase(2, 0.f, 4, 2.f);
    EXPECT_NEAR(p, 0.f, 1e-6f);
}

TEST(BeatMath_BarPhase, ZeroBeatsPerBar_ReturnsZero)
{
    float p = mts::BeatMath::BarPhase(2, 0.5f, 0, 1.f);
    EXPECT_FLOAT_EQ(p, 0.f);
}

// =============================================================================
// Group B -- ConductorSystem state tests (use EngineTestFixture)
// =============================================================================

class ConductorSystemTest : public EngineTestFixture {};

TEST_F(ConductorSystemTest, InitialState_IsZeroed)
{
    mts::ConductorSystem& cs = mts::ConductorSystem::Get();
    EXPECT_EQ(cs.GetBeatIndex(), 0);
    EXPECT_EQ(cs.GetHalfBeatIndex(), 0);
    EXPECT_EQ(cs.GetBarIndex(), 0);
    EXPECT_EQ(cs.GetBeatInBar(), 0);
    EXPECT_NEAR(cs.GetBeatPhase(), 0.f, 1e-6f);
    EXPECT_FALSE(cs.IsMusicPlaying());
}

TEST_F(ConductorSystemTest, ResetState_WhenMusicStopped)
{
    mts::ConductorSystem& cs = mts::ConductorSystem::Get();
    // Update without music playing: state should stay zeroed.
    cs.Update(0.f);
    EXPECT_EQ(cs.GetBeatIndex(), 0);
    EXPECT_FALSE(cs.IsMusicPlaying());
}

TEST_F(ConductorSystemTest, OnBeat_FiresOnBeatCrossing)
{
    mts::AudioManager& audio = mts::AudioManager::Get();
    mts::ConductorSystem& cs = mts::ConductorSystem::Get();

    // Use BPM override so we fully control beat timing without AudioManager rhythm bookkeeping.
    cs.SetBPM(120.f);
    cs.SetOffsetSec(0.f);

    int beatCount = 0;
    cs.onBeat.Subscribe([&](const mts::BeatInfo& info) {
        if (!info.isHalfBeat)
            ++beatCount;
    });

    // Start music (StubBackend always returns a valid handle).
    mts::MusicTrack track;
    track.trackId = "test";
    track.eventId = "test_event";
    track.bpm = 120.f;
    track.beatsPerBar = 4;
    track.offsetSec = 0.f;
    audio.StartMusic(track);

    // Simulate time advancing past at least 2 full beats (each 0.5 s at 120 BPM).
    // Update in small steps so crossing detection fires.
    const float dt = 0.05f;
    for (int i = 0; i < 40; ++i)   // 40 * 0.05s = 2.0s = 4 beats at 120 BPM
    {
        audio.Update(dt);
        cs.Update(dt);
    }

    EXPECT_GT(beatCount, 0);
}

TEST_F(ConductorSystemTest, OnBar_FiresOnlyOnDownbeat)
{
    mts::AudioManager& audio = mts::AudioManager::Get();
    mts::ConductorSystem& cs = mts::ConductorSystem::Get();

    cs.SetBPM(120.f);
    cs.SetOffsetSec(0.f);
    cs.SetBeatsPerBar(4);

    int barCount  = 0;
    int beatCount = 0;
    cs.onBar.Subscribe([&](const mts::BeatInfo&) { ++barCount; });
    cs.onBeat.Subscribe([&](const mts::BeatInfo& info) {
        if (!info.isHalfBeat)
            ++beatCount;
    });

    mts::MusicTrack track;
    track.trackId = "test";
    track.eventId = "test_event";
    track.bpm = 120.f;
    track.beatsPerBar = 4;
    track.offsetSec = 0.f;
    audio.StartMusic(track);

    // Advance 8 beats (2 bars) at 120 BPM = 4.0 s
    const float dt = 0.05f;
    for (int i = 0; i < 80; ++i)
    {
        audio.Update(dt);
        cs.Update(dt);
    }

    // Should have fired onBar roughly every 4 beats.
    EXPECT_GT(barCount, 0);
    // Bar count should be strictly less than beat count.
    EXPECT_LT(barCount, beatCount);
}

TEST_F(ConductorSystemTest, OverrideBPM_ChangesBeatsPerTime)
{
    mts::AudioManager& audio = mts::AudioManager::Get();

    // Helper lambda: count beats at a given BPM over 1 second.
    auto countBeats = [&](float bpm) -> int
    {
        // Fresh conductor for each run.
        mts::ConductorSystem cs2;
        mts::ConductorSystem::SetInstance(&cs2);
        cs2.Init();
        cs2.SetBPM(bpm);
        cs2.SetOffsetSec(0.f);

        int count = 0;
        cs2.onBeat.Subscribe([&](const mts::BeatInfo& info) {
            if (!info.isHalfBeat)
                ++count;
        });

        mts::MusicTrack track;
        track.trackId = "test";
        track.eventId = "test_event";
        track.bpm = bpm;
        track.beatsPerBar = 4;
        track.offsetSec = 0.f;
        audio.StartMusic(track);

        const float dt = 0.01f;
        for (int i = 0; i < 100; ++i)   // 1.0 s
        {
            audio.Update(dt);
            cs2.Update(dt);
        }

        audio.StopMusic();
        cs2.Shutdown();
        mts::ConductorSystem::SetInstance(&mConductor);
        return count;
    };

    int beats120 = countBeats(120.f);  // ~2 beats/s
    int beats240 = countBeats(240.f);  // ~4 beats/s

    EXPECT_GT(beats240, beats120);
}

#include "pch.h"
#include "Engine/Conductor/BeatMath.h"

// All tests call BeatMath::BarPhase directly -- pure arithmetic, no fixture needed.

TEST(BeatBarController_BarPhase, Beat0_IsZero)
{
    float p = mts::BeatMath::BarPhase(0, 0.f, 4, 1.f);
    EXPECT_FLOAT_EQ(p, 0.f);
}

TEST(BeatBarController_BarPhase, EndOfBar_ApproachesOne)
{
    // beatInBar=3, beatPhase≈1.0, 4/4, multiplier=1.
    // base = (3 + 0.9999f) / 4 ≈ 0.99997; fmod ≈ 0.99997 (< 1).
    float p = mts::BeatMath::BarPhase(3, 0.9999f, 4, 1.f);
    EXPECT_GT(p, 0.99f);
    EXPECT_LT(p, 1.f);
}

TEST(BeatBarController_BarPhase, Multiplier2_HalfBarWraps)
{
    // beatInBar=2, beatPhase=0, 4/4, multiplier=2.
    // base = 2/4 = 0.5; 0.5 * 2 = 1.0; fmod(1.0, 1.0) = 0.0.
    float p = mts::BeatMath::BarPhase(2, 0.f, 4, 2.f);
    EXPECT_NEAR(p, 0.f, 1e-6f);
}

TEST(BeatBarController_BarPhase, MidBar_Correct)
{
    // beatInBar=2, beatPhase=0, 4/4, multiplier=1 => 2/4 = 0.5
    float p = mts::BeatMath::BarPhase(2, 0.f, 4, 1.f);
    EXPECT_NEAR(p, 0.5f, 1e-6f);
}

TEST(BeatBarController_BarPhase, SpeedMultiplier1_NoChange)
{
    // multiplier=1 is identity: result equals base phase.
    float p = mts::BeatMath::BarPhase(1, 0.f, 4, 1.f);
    EXPECT_NEAR(p, 0.25f, 1e-6f);
}

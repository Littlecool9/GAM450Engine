#include "pch.h"
#include "Engine/Input/Input.h"
#include <gtest/gtest.h>

// =============================================================================
// Fixture — initialises and shuts down Input around each test.
// =============================================================================

class GamepadInputTest : public ::testing::Test
{
   protected:
    void SetUp() override { mts::Input::Init(); }
    void TearDown() override { mts::Input::Shutdown(); }

    // Simulate advancing one frame: save current state as previous.
    static void NextFrame() { mts::Input::BeginFrame(); }
};

// =============================================================================
// Helper — build a zeroed button array with specific buttons set.
// =============================================================================

static constexpr int kButtonCount = static_cast<int>(mts::GamepadButton::Count);
static constexpr int kAxisCount   = static_cast<int>(mts::GamepadAxis::Count);

struct FakeGamepadState
{
    bool  buttons[kButtonCount]{};
    float axes[kAxisCount]{};
};

static void PollPad(int padId, const FakeGamepadState& s)
{
    mts::Input::OnGamepadPoll(padId, s.buttons, kButtonCount, s.axes, kAxisCount);
}

// =============================================================================
// Connect / disconnect
// =============================================================================

TEST_F(GamepadInputTest, NotConnectedByDefault)
{
    EXPECT_FALSE(mts::Input::IsGamepadConnected(0));
}

TEST_F(GamepadInputTest, ConnectedAfterOnGamepadConnected)
{
    mts::Input::OnGamepadConnected(0);
    EXPECT_TRUE(mts::Input::IsGamepadConnected(0));
}

TEST_F(GamepadInputTest, DisconnectedAfterOnGamepadDisconnected)
{
    mts::Input::OnGamepadConnected(0);
    mts::Input::OnGamepadDisconnected(0);
    EXPECT_FALSE(mts::Input::IsGamepadConnected(0));
}

TEST_F(GamepadInputTest, DisconnectClearsButtonState)
{
    mts::Input::OnGamepadConnected(0);

    FakeGamepadState s;
    s.buttons[static_cast<int>(mts::GamepadButton::A)] = true;
    PollPad(0, s);

    mts::Input::OnGamepadDisconnected(0);
    EXPECT_FALSE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::A, 0));
}

TEST_F(GamepadInputTest, DisconnectClearsAxisState)
{
    mts::Input::OnGamepadConnected(0);

    FakeGamepadState s;
    s.axes[static_cast<int>(mts::GamepadAxis::LeftX)] = 0.9f;
    PollPad(0, s);

    mts::Input::OnGamepadDisconnected(0);
    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::LeftX, 0), 0.0f);
}

// =============================================================================
// Button IsDown / IsPressed / IsReleased
// =============================================================================

TEST_F(GamepadInputTest, ButtonDownAfterPoll)
{
    mts::Input::OnGamepadConnected(0);

    FakeGamepadState s;
    s.buttons[static_cast<int>(mts::GamepadButton::A)] = true;
    PollPad(0, s);

    EXPECT_TRUE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::A, 0));
    EXPECT_FALSE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::B, 0));
}

TEST_F(GamepadInputTest, ButtonPressedOnlyOnFirstFrame)
{
    mts::Input::OnGamepadConnected(0);

    // Frame 1 — button goes down.
    NextFrame();
    FakeGamepadState s;
    s.buttons[static_cast<int>(mts::GamepadButton::A)] = true;
    PollPad(0, s);

    EXPECT_TRUE(mts::Input::IsGamepadButtonPressed(mts::GamepadButton::A, 0));

    // Frame 2 — button still held.
    NextFrame();
    PollPad(0, s);

    EXPECT_FALSE(mts::Input::IsGamepadButtonPressed(mts::GamepadButton::A, 0));
    EXPECT_TRUE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::A, 0));
}

TEST_F(GamepadInputTest, ButtonReleasedOnlyOnReleaseFrame)
{
    mts::Input::OnGamepadConnected(0);

    // Frame 1 — press.
    NextFrame();
    FakeGamepadState held;
    held.buttons[static_cast<int>(mts::GamepadButton::B)] = true;
    PollPad(0, held);

    // Frame 2 — release.
    NextFrame();
    FakeGamepadState released;  // all buttons false
    PollPad(0, released);

    EXPECT_TRUE(mts::Input::IsGamepadButtonReleased(mts::GamepadButton::B, 0));

    // Frame 3 — still released, should no longer trigger.
    NextFrame();
    PollPad(0, released);

    EXPECT_FALSE(mts::Input::IsGamepadButtonReleased(mts::GamepadButton::B, 0));
}

// =============================================================================
// Axis
// =============================================================================

TEST_F(GamepadInputTest, AxisDefaultIsZero)
{
    mts::Input::OnGamepadConnected(0);
    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::LeftX, 0), 0.0f);
}

TEST_F(GamepadInputTest, AxisReturnsPolledValue)
{
    mts::Input::OnGamepadConnected(0);

    FakeGamepadState s;
    s.axes[static_cast<int>(mts::GamepadAxis::RightY)] = -0.75f;
    PollPad(0, s);

    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::RightY, 0), -0.75f);
}

TEST_F(GamepadInputTest, TriggerAxisRange)
{
    mts::Input::OnGamepadConnected(0);

    FakeGamepadState s;
    s.axes[static_cast<int>(mts::GamepadAxis::LeftTrigger)]  =  1.0f;
    s.axes[static_cast<int>(mts::GamepadAxis::RightTrigger)] = -1.0f;
    PollPad(0, s);

    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::LeftTrigger, 0),   1.0f);
    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::RightTrigger, 0), -1.0f);
}

// =============================================================================
// ClearAll
// =============================================================================

TEST_F(GamepadInputTest, ClearAllZeroesButtonsAndAxes)
{
    mts::Input::OnGamepadConnected(0);

    FakeGamepadState s;
    s.buttons[static_cast<int>(mts::GamepadButton::Start)] = true;
    s.axes[static_cast<int>(mts::GamepadAxis::LeftX)] = 0.5f;
    PollPad(0, s);

    mts::Input::ClearAll();

    EXPECT_FALSE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::Start, 0));
    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::LeftX, 0), 0.0f);
}

TEST_F(GamepadInputTest, ClearAllLeavesConnectedFlag)
{
    mts::Input::OnGamepadConnected(0);
    mts::Input::ClearAll();
    EXPECT_TRUE(mts::Input::IsGamepadConnected(0));
}

// =============================================================================
// Multi-pad
// =============================================================================

TEST_F(GamepadInputTest, IndependentPadState)
{
    mts::Input::OnGamepadConnected(0);
    mts::Input::OnGamepadConnected(1);

    NextFrame();

    FakeGamepadState s0, s1;
    s0.buttons[static_cast<int>(mts::GamepadButton::X)] = true;
    s1.buttons[static_cast<int>(mts::GamepadButton::Y)] = true;
    PollPad(0, s0);
    PollPad(1, s1);

    EXPECT_TRUE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::X, 0));
    EXPECT_FALSE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::Y, 0));
    EXPECT_FALSE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::X, 1));
    EXPECT_TRUE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::Y, 1));
}

// =============================================================================
// Out-of-range safety
// =============================================================================

TEST_F(GamepadInputTest, OutOfRangePadIdReturnsSafeDefaults)
{
    EXPECT_FALSE(mts::Input::IsGamepadConnected(-1));
    EXPECT_FALSE(mts::Input::IsGamepadConnected(mts::Input::MaxGamepads));
    EXPECT_FALSE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::A, -1));
    EXPECT_FALSE(mts::Input::IsGamepadButtonPressed(mts::GamepadButton::A, -1));
    EXPECT_FALSE(mts::Input::IsGamepadButtonReleased(mts::GamepadButton::A, -1));
    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::LeftX, -1), 0.0f);
}

TEST_F(GamepadInputTest, UnknownButtonReturnsFalse)
{
    mts::Input::OnGamepadConnected(0);
    EXPECT_FALSE(mts::Input::IsGamepadButtonDown(mts::GamepadButton::Unknown, 0));
    EXPECT_FALSE(mts::Input::IsGamepadButtonPressed(mts::GamepadButton::Unknown, 0));
}

TEST_F(GamepadInputTest, UnknownAxisReturnsZero)
{
    mts::Input::OnGamepadConnected(0);
    EXPECT_FLOAT_EQ(mts::Input::GetGamepadAxis(mts::GamepadAxis::Unknown, 0), 0.0f);
}

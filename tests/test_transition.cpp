#include "Transition.hpp"

#include "mocks/MockBuzzer.hpp"
#include "mocks/MockLed.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace warden;
using namespace warden::test;
using ::testing::StrictMock;

class TransitionTest : public ::testing::Test {
protected:
    StrictMock<MockLed>    led;
    StrictMock<MockBuzzer> buzzer;
};

TEST_F(TransitionTest, NormalGreenSteadyLedNoBuzzer) {
    EXPECT_CALL(led, setMode(LedColor::Green, false));
    applyTransition(led, buzzer, {.from = State::Warning, .to = State::Normal});
}

TEST_F(TransitionTest, WarningOrangeSteadyLedNoBuzzer) {
    EXPECT_CALL(led, setMode(LedColor::Orange, false));
    applyTransition(led, buzzer, {.from = State::Normal, .to = State::Warning});
}

TEST_F(TransitionTest, EnteringAlertRedBlinkingLedThreeShortBeeps) {
    EXPECT_CALL(led, setMode(LedColor::Red, true));
    EXPECT_CALL(buzzer, shortBeep(3));
    applyTransition(led, buzzer, {.from = State::Normal, .to = State::Alert});
}

TEST_F(TransitionTest, LeavingAlertOneLongBeep) {
    EXPECT_CALL(led, setMode(LedColor::Green, false));
    EXPECT_CALL(buzzer, longBeep(1));
    applyTransition(led, buzzer, {.from = State::Alert, .to = State::Normal});
}

TEST_F(TransitionTest, WarningToAlertRedBlinkingLedThreeShortBeeps) {
    EXPECT_CALL(led, setMode(LedColor::Red, true));
    EXPECT_CALL(buzzer, shortBeep(3));
    applyTransition(led, buzzer, {.from = State::Warning, .to = State::Alert});
}

TEST_F(TransitionTest, AlertToWarningOrangeSteadyLedOneLongBeep) {
    EXPECT_CALL(led, setMode(LedColor::Orange, false));
    EXPECT_CALL(buzzer, longBeep(1));
    applyTransition(led, buzzer, {.from = State::Alert, .to = State::Warning});
}

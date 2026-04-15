#include "Config.hpp"
#include "WardenApp.hpp"

#include "mocks/MockBuzzer.hpp"
#include "mocks/MockLed.hpp"
#include "mocks/MockSensor.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace warden;
using namespace warden::test;
using ::testing::StrictMock;

class WardenAppTest : public ::testing::Test {
protected:
    StrictMock<MockSensor> sensor;
    StrictMock<MockLed>    led;
    StrictMock<MockBuzzer> buzzer;
    Config                 config;
    WardenApp              app{sensor, led, buzzer, config};
};

TEST_F(WardenAppTest, NormalGreenSteadyLedNoBuzzer) {
    EXPECT_CALL(led, setMode(LedColor::Green, false));
    app.applyTransition({.from = State::Warning, .to = State::Normal});
}

TEST_F(WardenAppTest, WarningOrangeSteadyLedNoBuzzer) {
    EXPECT_CALL(led, setMode(LedColor::Orange, false));
    app.applyTransition({.from = State::Normal, .to = State::Warning});
}

TEST_F(WardenAppTest, EnteringAlertRedBlinkingLedThreeShortBeeps) {
    EXPECT_CALL(led, setMode(LedColor::Red, true));
    EXPECT_CALL(buzzer, shortBeep(3));
    app.applyTransition({.from = State::Normal, .to = State::Alert});
}

TEST_F(WardenAppTest, LeavingAlertOneLongBeep) {
    EXPECT_CALL(led, setMode(LedColor::Green, false));
    EXPECT_CALL(buzzer, longBeep(1));
    app.applyTransition({.from = State::Alert, .to = State::Normal});
}

TEST_F(WardenAppTest, WarningToAlertRedBlinkingLedThreeShortBeeps) {
    EXPECT_CALL(led, setMode(LedColor::Red, true));
    EXPECT_CALL(buzzer, shortBeep(3));
    app.applyTransition({.from = State::Warning, .to = State::Alert});
}

TEST_F(WardenAppTest, AlertToWarningOrangeSteadyLedOneLongBeep) {
    EXPECT_CALL(led, setMode(LedColor::Orange, false));
    EXPECT_CALL(buzzer, longBeep(1));
    app.applyTransition({.from = State::Alert, .to = State::Warning});
}

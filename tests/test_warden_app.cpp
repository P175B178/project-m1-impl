#include <catch2/catch_test_macros.hpp>

#include "Config.hpp"
#include "WardenApp.hpp"

#include "mocks/MockBuzzer.hpp"
#include "mocks/MockLed.hpp"
#include "mocks/MockSensor.hpp"

using namespace warden;
using namespace warden::test;

// Constructs a WardenApp with default config and the provided mocks.
static WardenApp makeApp(MockSensor& s, MockLed& l, MockBuzzer& b) {
    Config config;
    return WardenApp{s, l, b, config};
}

TEST_CASE("applyTransition — Normal: green steady LED, no buzzer") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({.from = State::Warning, .to = State::Normal});

    REQUIRE(l.getLastColor() == LedColor::Green);
    REQUIRE(l.getLastBlink() == false);
    REQUIRE(b.getCallCount() == 0);
}

TEST_CASE("applyTransition — Warning: orange steady LED, no buzzer") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({.from = State::Normal, .to = State::Warning});

    REQUIRE(l.getLastColor() == LedColor::Orange);
    REQUIRE(l.getLastBlink() == false);
    REQUIRE(b.getCallCount() == 0);
}

TEST_CASE("applyTransition — entering Alert: red blinking LED, three short beeps") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({.from = State::Normal, .to = State::Alert});

    REQUIRE(l.getLastColor() == LedColor::Red);
    REQUIRE(l.getLastBlink() == true);
    REQUIRE(b.getCallCount() == 1);
    REQUIRE(b.getLastCount() == 3);
    REQUIRE(b.getLastDurationMs() == 100);
    REQUIRE(b.getLastPauseMs() == 100);
}

TEST_CASE("applyTransition — leaving Alert: one long beep") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({.from = State::Alert, .to = State::Normal});

    REQUIRE(b.getCallCount() == 1);
    REQUIRE(b.getLastCount() == 1);
    REQUIRE(b.getLastDurationMs() == 500);
    REQUIRE(b.getLastPauseMs() == 0);
}

TEST_CASE("applyTransition — Warning to Alert: red blinking LED, three short beeps") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({.from = State::Warning, .to = State::Alert});

    REQUIRE(l.getLastColor() == LedColor::Red);
    REQUIRE(l.getLastBlink() == true);
    REQUIRE(b.getCallCount() == 1);
    REQUIRE(b.getLastCount() == 3);
}

TEST_CASE("applyTransition — Alert to Warning: orange steady LED, one long beep") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({.from = State::Alert, .to = State::Warning});

    REQUIRE(l.getLastColor() == LedColor::Orange);
    REQUIRE(l.getLastBlink() == false);
    REQUIRE(b.getCallCount() == 1);
    REQUIRE(b.getLastCount() == 1);
    REQUIRE(b.getLastDurationMs() == 500);
}

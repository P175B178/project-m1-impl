#include <catch2/catch_test_macros.hpp>

#include "warden/Config.hpp"
#include "warden/WardenApp.hpp"

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

    app.applyTransition({State::Warning, State::Normal});

    REQUIRE(l.lastColor() == LedColor::Green);
    REQUIRE(l.lastBlink() == false);
    REQUIRE(b.callCount() == 0);
}

TEST_CASE("applyTransition — Warning: orange steady LED, no buzzer") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({State::Normal, State::Warning});

    REQUIRE(l.lastColor() == LedColor::Orange);
    REQUIRE(l.lastBlink() == false);
    REQUIRE(b.callCount() == 0);
}

TEST_CASE("applyTransition — entering Alert: red blinking LED, three short beeps") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({State::Normal, State::Alert});

    REQUIRE(l.lastColor() == LedColor::Red);
    REQUIRE(l.lastBlink() == true);
    REQUIRE(b.callCount() == 1);
    REQUIRE(b.lastCount() == 3);
    REQUIRE(b.lastDurationMs() == 100);
    REQUIRE(b.lastPauseMs() == 100);
}

TEST_CASE("applyTransition — leaving Alert: one long beep") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({State::Alert, State::Normal});

    REQUIRE(b.callCount() == 1);
    REQUIRE(b.lastCount() == 1);
    REQUIRE(b.lastDurationMs() == 500);
    REQUIRE(b.lastPauseMs() == 0);
}

TEST_CASE("applyTransition — Warning to Alert: red blinking LED, three short beeps") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({State::Warning, State::Alert});

    REQUIRE(l.lastColor() == LedColor::Red);
    REQUIRE(l.lastBlink() == true);
    REQUIRE(b.callCount() == 1);
    REQUIRE(b.lastCount() == 3);
}

TEST_CASE("applyTransition — Alert to Warning: orange steady LED, one long beep") {
    MockSensor s; MockLed l; MockBuzzer b;
    auto app = makeApp(s, l, b);

    app.applyTransition({State::Alert, State::Warning});

    REQUIRE(l.lastColor() == LedColor::Orange);
    REQUIRE(l.lastBlink() == false);
    REQUIRE(b.callCount() == 1);
    REQUIRE(b.lastCount() == 1);
    REQUIRE(b.lastDurationMs() == 500);
}

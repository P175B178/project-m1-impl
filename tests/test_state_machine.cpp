#include "warden/StateMachine.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <tuple>

using namespace warden;

static constexpr float TEMP_THRESHOLD = 28.0f;
static constexpr float HUM_THRESHOLD  = 70.0f;

TEST_CASE("StateMachine — starts in Normal", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    REQUIRE(sm.currentState() == State::Normal);
}

TEST_CASE("StateMachine — stays Normal when both thresholds are not exceeded", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    auto t = sm.update(20.0f, 50.0f);
    REQUIRE_FALSE(t.has_value());
    REQUIRE(sm.currentState() == State::Normal);
}

TEST_CASE("StateMachine — Normal → Warning when temperature only is exceeded", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    auto t = sm.update(30.0f, 50.0f);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Normal);
    REQUIRE(t->to   == State::Warning);
    REQUIRE(sm.currentState() == State::Warning);
}

TEST_CASE("StateMachine — Normal → Warning when humidity only is exceeded", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    auto t = sm.update(20.0f, 80.0f);
    REQUIRE(t.has_value());
    REQUIRE(t->to == State::Warning);
}

TEST_CASE("StateMachine — Normal → Alert when both thresholds are exceeded", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    auto t = sm.update(30.0f, 80.0f);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Normal);
    REQUIRE(t->to   == State::Alert);
    REQUIRE(sm.currentState() == State::Alert);
}

TEST_CASE("StateMachine — Warning → Alert when second threshold exceeded", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    std::ignore = sm.update(30.0f, 50.0f);  // → Warning
    auto t = sm.update(30.0f, 80.0f);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Warning);
    REQUIRE(t->to   == State::Alert);
}

TEST_CASE("StateMachine — Alert → Warning when one threshold drops below", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    std::ignore = sm.update(30.0f, 80.0f);  // → Alert
    auto t = sm.update(20.0f, 80.0f);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Alert);
    REQUIRE(t->to   == State::Warning);
}

TEST_CASE("StateMachine — Alert → Normal when both thresholds drop below", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    std::ignore = sm.update(30.0f, 80.0f);  // → Alert
    auto t = sm.update(20.0f, 50.0f);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Alert);
    REQUIRE(t->to   == State::Normal);
}

TEST_CASE("StateMachine — no transition when state stays the same", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    std::ignore = sm.update(30.0f, 80.0f);  // → Alert
    auto t = sm.update(31.0f, 85.0f);  // still Alert
    REQUIRE_FALSE(t.has_value());
}

TEST_CASE("StateMachine — values exactly at threshold are not exceeded", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    auto t = sm.update(TEMP_THRESHOLD, HUM_THRESHOLD);
    REQUIRE_FALSE(t.has_value());
    REQUIRE(sm.currentState() == State::Normal);
}

TEST_CASE("StateMachine — thresholds can be updated at runtime", "[state_machine]") {
    StateMachine sm{TEMP_THRESHOLD, HUM_THRESHOLD};
    sm.setTemperatureThreshold(25.0f);
    auto t = sm.update(26.0f, 50.0f);  // now exceeds new threshold
    REQUIRE(t.has_value());
    REQUIRE(t->to == State::Warning);
}

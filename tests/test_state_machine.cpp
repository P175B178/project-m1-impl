#include "warden/StateMachine.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <tuple>

using namespace warden;

static constexpr float tempThreshold = 28.0F;
static constexpr float humThreshold  = 70.0F;

TEST_CASE("StateMachine — starts in Normal", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    REQUIRE(sm.currentState() == State::Normal);
}

TEST_CASE("StateMachine — stays Normal when both thresholds are not exceeded", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    auto t = sm.update(20.0F, 50.0F);
    REQUIRE_FALSE(t.has_value());
    REQUIRE(sm.currentState() == State::Normal);
}

TEST_CASE("StateMachine — Normal → Warning when temperature only is exceeded", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    auto t = sm.update(30.0F, 50.0F);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Normal);
    REQUIRE(t->to   == State::Warning);
    REQUIRE(sm.currentState() == State::Warning);
}

TEST_CASE("StateMachine — Normal → Warning when humidity only is exceeded", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    auto t = sm.update(20.0F, 80.0F);
    REQUIRE(t.has_value());
    REQUIRE(t->to == State::Warning);
}

TEST_CASE("StateMachine — Normal → Alert when both thresholds are exceeded", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    auto t = sm.update(30.0F, 80.0F);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Normal);
    REQUIRE(t->to   == State::Alert);
    REQUIRE(sm.currentState() == State::Alert);
}

TEST_CASE("StateMachine — Warning → Alert when second threshold exceeded", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    std::ignore = sm.update(30.0F, 50.0F);  // → Warning
    auto t = sm.update(30.0F, 80.0F);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Warning);
    REQUIRE(t->to   == State::Alert);
}

TEST_CASE("StateMachine — Alert → Warning when one threshold drops below", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    std::ignore = sm.update(30.0F, 80.0F);  // → Alert
    auto t = sm.update(20.0F, 80.0F);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Alert);
    REQUIRE(t->to   == State::Warning);
}

TEST_CASE("StateMachine — Alert → Normal when both thresholds drop below", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    std::ignore = sm.update(30.0F, 80.0F);  // → Alert
    auto t = sm.update(20.0F, 50.0F);
    REQUIRE(t.has_value());
    REQUIRE(t->from == State::Alert);
    REQUIRE(t->to   == State::Normal);
}

TEST_CASE("StateMachine — no transition when state stays the same", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    std::ignore = sm.update(30.0F, 80.0F);  // → Alert
    auto t = sm.update(31.0F, 85.0F);  // still Alert
    REQUIRE_FALSE(t.has_value());
}

TEST_CASE("StateMachine — values exactly at threshold are not exceeded", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    auto t = sm.update(tempThreshold, humThreshold);
    REQUIRE_FALSE(t.has_value());
    REQUIRE(sm.currentState() == State::Normal);
}

TEST_CASE("StateMachine — thresholds can be updated at runtime", "[state_machine]") {
    StateMachine sm{tempThreshold, humThreshold};
    sm.setTemperatureThreshold(25.0F);
    auto t = sm.update(26.0F, 50.0F);  // now exceeds new threshold
    REQUIRE(t.has_value());
    REQUIRE(t->to == State::Warning);
}

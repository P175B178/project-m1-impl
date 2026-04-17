#include "StateMachine.hpp"

#include <gtest/gtest.h>

using namespace warden;

static constexpr float tempThreshold = 28.0F;
static constexpr float humThreshold  = 70.0F;

TEST(StateMachine, StartsInNormal) {
  StateMachine sm{tempThreshold, humThreshold};
  EXPECT_EQ(sm.currentState(), State::Normal);
}

TEST(StateMachine, StaysNormalWhenBothThresholdsNotExceeded) {
  StateMachine sm{tempThreshold, humThreshold};
  auto t = sm.update({.temperature = 20.0F, .humidity = 50.0F});
  EXPECT_FALSE(t.has_value());
  EXPECT_EQ(sm.currentState(), State::Normal);
}

TEST(StateMachine, NormalToWarningWhenTemperatureOnlyExceeded) {
  StateMachine sm{tempThreshold, humThreshold};
  auto t = sm.update({.temperature = 30.0F, .humidity = 50.0F});
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(t->from, State::Normal);
  EXPECT_EQ(t->to, State::Warning);
  EXPECT_EQ(sm.currentState(), State::Warning);
}

TEST(StateMachine, NormalToWarningWhenHumidityOnlyExceeded) {
  StateMachine sm{tempThreshold, humThreshold};
  auto t = sm.update({.temperature = 20.0F, .humidity = 80.0F});
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(t->to, State::Warning);
}

TEST(StateMachine, NormalToAlertWhenBothThresholdsExceeded) {
  StateMachine sm{tempThreshold, humThreshold};
  auto t = sm.update({.temperature = 30.0F, .humidity = 80.0F});
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(t->from, State::Normal);
  EXPECT_EQ(t->to, State::Alert);
  EXPECT_EQ(sm.currentState(), State::Alert);
}

TEST(StateMachine, WarningToAlertWhenSecondThresholdExceeded) {
  StateMachine sm{tempThreshold, humThreshold};
  std::ignore = sm.update({.temperature = 30.0F, .humidity = 50.0F}); // -> Warning
  auto t      = sm.update({.temperature = 30.0F, .humidity = 80.0F});
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(t->from, State::Warning);
  EXPECT_EQ(t->to, State::Alert);
}

TEST(StateMachine, AlertToWarningWhenOneThresholdDropsBelow) {
  StateMachine sm{tempThreshold, humThreshold};
  std::ignore = sm.update({.temperature = 30.0F, .humidity = 80.0F}); // -> Alert
  auto t      = sm.update({.temperature = 20.0F, .humidity = 80.0F});
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(t->from, State::Alert);
  EXPECT_EQ(t->to, State::Warning);
}

TEST(StateMachine, AlertToNormalWhenBothThresholdsDropBelow) {
  StateMachine sm{tempThreshold, humThreshold};
  std::ignore = sm.update({.temperature = 30.0F, .humidity = 80.0F}); // -> Alert
  auto t      = sm.update({.temperature = 20.0F, .humidity = 50.0F});
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(t->from, State::Alert);
  EXPECT_EQ(t->to, State::Normal);
}

TEST(StateMachine, NoTransitionWhenStateStaysTheSame) {
  StateMachine sm{tempThreshold, humThreshold};
  std::ignore = sm.update({.temperature = 30.0F, .humidity = 80.0F}); // -> Alert
  auto t      = sm.update({.temperature = 31.0F, .humidity = 85.0F}); // still Alert
  EXPECT_FALSE(t.has_value());
}

TEST(StateMachine, ValuesExactlyAtThresholdAreNotExceeded) {
  StateMachine sm{tempThreshold, humThreshold};
  auto t = sm.update({.temperature = tempThreshold, .humidity = humThreshold});
  EXPECT_FALSE(t.has_value());
  EXPECT_EQ(sm.currentState(), State::Normal);
}

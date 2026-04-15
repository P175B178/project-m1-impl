#include "warden/StateMachine.hpp"

#include <spdlog/spdlog.h>

namespace warden {

StateMachine::StateMachine(float temperatureThreshold, float humidityThreshold)
    : temperatureThreshold{temperatureThreshold}, humidityThreshold{humidityThreshold} {}

std::optional<StateTransition> StateMachine::update(float temperature, float humidity) {
  const State newState = evaluate(temperature, humidity);
  if (newState == state) {
    return std::nullopt;
  }

  StateTransition transition{.from = state, .to = newState};
  state = newState;
  return transition;
}

State StateMachine::evaluate(float temperature, float humidity) const noexcept {
  const bool tempExceeded = temperature > temperatureThreshold;
  const bool humExceeded = humidity > humidityThreshold;

  spdlog::debug("StateMachine: temp={:.1f} (threshold={:.1f}, exceeded={}) hum={:.1f} (threshold={:.1f}, exceeded={})",
                temperature, temperatureThreshold, tempExceeded, humidity, humidityThreshold, humExceeded);

  if (tempExceeded && humExceeded) {
    return State::Alert;
  }
  if (tempExceeded || humExceeded) {
    return State::Warning;
  }
  return State::Normal;
}

} // namespace warden

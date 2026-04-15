#include "StateMachine.hpp"

#include <spdlog/spdlog.h>

namespace warden {

StateMachine::StateMachine(float temperatureThreshold, float humidityThreshold) // NOLINT(bugprone-easily-swappable-parameters)
    : temperatureThreshold{temperatureThreshold}, humidityThreshold{humidityThreshold} {}

std::optional<StateTransition> StateMachine::update(Sample sample) {
  const State newState = evaluate(sample);
  if (newState == state) {
    return std::nullopt;
  }

  StateTransition transition{.from = state, .to = newState};
  state = newState;
  return transition;
}

State StateMachine::evaluate(Sample sample) const noexcept {
  const bool tempExceeded = sample.temperature > temperatureThreshold;
  const bool humExceeded  = sample.humidity > humidityThreshold;

  spdlog::debug("StateMachine: temp={:.1f} (threshold={:.1f}, exceeded={}) hum={:.1f} (threshold={:.1f}, exceeded={})",
                sample.temperature, temperatureThreshold, tempExceeded, sample.humidity, humidityThreshold, humExceeded);

  if (tempExceeded && humExceeded) {
    return State::Alert;
  }
  if (tempExceeded || humExceeded) {
    return State::Warning;
  }
  return State::Normal;
}

} // namespace warden

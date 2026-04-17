#include "StateMachine.hpp"

StateMachine::StateMachine(float temperatureThreshold, float humidityThreshold)
      : temperatureThreshold{temperatureThreshold}
      , humidityThreshold{humidityThreshold} {}

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

  if (tempExceeded && humExceeded) {
    return State::Alert;
  }
  if (tempExceeded || humExceeded) {
    return State::Warning;
  }
  return State::Normal;
}

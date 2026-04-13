#include "warden/StateMachine.hpp"

#include <spdlog/spdlog.h>

namespace warden {

StateMachine::StateMachine(float temperatureThreshold, float humidityThreshold)
    : temperatureThreshold_{temperatureThreshold}, humidityThreshold_{humidityThreshold} {}

std::optional<StateTransition> StateMachine::update(float temperature, float humidity) {
  const State newState = evaluate(temperature, humidity);
  if (newState == state_) {
    return std::nullopt;
  }

  StateTransition transition{state_, newState};
  state_ = newState;
  return transition;
}

State StateMachine::evaluate(float temperature, float humidity) const noexcept {
  const bool tempExceeded = temperature > temperatureThreshold_;
  const bool humExceeded = humidity > humidityThreshold_;

  spdlog::debug("StateMachine: temp={:.1f} (threshold={:.1f}, exceeded={}) hum={:.1f} (threshold={:.1f}, exceeded={})",
                temperature, temperatureThreshold_, tempExceeded, humidity, humidityThreshold_, humExceeded);

  if (tempExceeded && humExceeded) {
    return State::Alert;
  }
  if (tempExceeded || humExceeded) {
    return State::Warning;
  }
  return State::Normal;
}

} // namespace warden

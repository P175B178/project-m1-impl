#pragma once

#include "State.hpp"

#include <optional>

namespace warden {

/// Holds a state transition — only produced when the state actually changes.
struct StateTransition {
  State from;
  State to;
};

/// Pure state machine — no hardware dependencies.
///
/// Call update() after each averaged sensor reading. It computes the new
/// state and returns a transition only if the state changed.
class StateMachine {
public:
  explicit StateMachine(float temperatureThreshold, float humidityThreshold);

  /// Feed averaged temperature and humidity. Returns a transition if
  /// the state changed, std::nullopt if it stayed the same.
  [[nodiscard]] std::optional<StateTransition> update(float temperature, float humidity);

  [[nodiscard]] State currentState() const noexcept { return state_; }

  // Thresholds can be updated at runtime to support future remote reconfiguration.
  void setTemperatureThreshold(float t) noexcept { temperatureThreshold_ = t; }
  void setHumidityThreshold(float h) noexcept { humidityThreshold_ = h; }

private:
  [[nodiscard]] State evaluate(float temperature, float humidity) const noexcept;

  float temperatureThreshold_;
  float humidityThreshold_;
  State state_{State::Normal};
};

} // namespace warden

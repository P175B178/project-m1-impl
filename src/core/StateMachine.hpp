#pragma once

#include "State.hpp"

#include <optional>

namespace warden {

/// Holds a state transition — only produced when the state actually changes.
struct StateTransition {
  State from;
  State to;
};

/// Averaged temperature and humidity ready for state evaluation.
struct Sample {
  float temperature{}; ///< Averaged temperature in °C
  float humidity{};    ///< Averaged relative humidity in %
};

/// Pure state machine — no hardware dependencies.
///
/// Call update() after each averaged sensor reading. It computes the new
/// state and returns a transition only if the state changed.
class StateMachine {
public:
  explicit StateMachine(float temperatureThreshold, // NOLINT(bugprone-easily-swappable-parameters)
                        float humidityThreshold);

  /// Feed an averaged sample. Returns a transition if the state changed,
  /// std::nullopt if it stayed the same.
  [[nodiscard]] std::optional<StateTransition> update(Sample sample);

  [[nodiscard]] State currentState() const noexcept { return state; }

private:
  [[nodiscard]] State evaluate(Sample sample) const noexcept;

  float temperatureThreshold;
  float humidityThreshold;
  State state{State::Normal};
};

} // namespace warden

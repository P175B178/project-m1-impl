#pragma once

#include "State.hpp"

#include <optional>

struct StateTransition {
  State from;
  State to;
};

struct Sample {
  float temperature{};
  float humidity{};
};

class StateMachine {
public:
  explicit StateMachine(float temperatureThreshold, // NOLINT(bugprone-easily-swappable-parameters)
                        float humidityThreshold);

  [[nodiscard]] std::optional<StateTransition> update(Sample sample);
  [[nodiscard]] State currentState() const noexcept { return state; }

private:
  [[nodiscard]] State evaluate(Sample sample) const noexcept;

  float temperatureThreshold;
  float humidityThreshold;
  State state{State::Normal};
};

#pragma once

#include "hardware/Reading.hpp"
#include "hardware/Sensor.hpp"

#include <chrono>
#include <cmath>
#include <numbers>
#include <random>

/// Simulated sensor that produces a slowly oscillating sine-wave signal.
/// Temperature crosses the default 28 C threshold, humidity crosses 70 %,
/// so the full state-machine path is exercised without hardware.
/// Occasionally produces out-of-range values to exercise validation.
class SimSensor final : public Sensor {
public:
  [[nodiscard]] std::expected<Reading, SensorError> read() const override {
    using namespace std::chrono;
    const auto now       = system_clock::now();
    const double elapsed = duration<double>(now.time_since_epoch()).count();

    // NOLINTBEGIN(readability-magic-numbers)
    constexpr double cyclePeriodSeconds = 6.0 * 60.0;

    const double phase = elapsed / cyclePeriodSeconds * (2.0 * std::numbers::pi);

    Reading reading;
    reading.timestamp   = now;
    reading.temperature = 24.0F + 8.0F * static_cast<float>(std::sin(phase));        // 16-32 C
    reading.humidity    = 55.0F + 20.0F * static_cast<float>(std::sin(phase * 0.7)); // 35-75 %

    // ~10% chance of producing an implausible reading
    switch (badValueDist(rng)) {
    case 0:
      reading.temperature = 999.0F;
      break;
    case 1:
      reading.humidity = -50.0F;
      break;
    default:
      break;
    }
    // NOLINTEND(readability-magic-numbers)

    return reading;
  }

private:
  // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
  mutable std::mt19937 rng{std::random_device{}()};
  mutable std::uniform_int_distribution<int> badValueDist{0, 9}; // NOLINT(readability-magic-numbers)
  // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
};

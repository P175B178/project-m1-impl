#pragma once

#include "hardware/Reading.hpp"
#include "hardware/Sensor.hpp"

#include <chrono>
#include <cmath>
#include <numbers>

namespace warden::sim {

/// Simulated sensor that produces a slowly oscillating sine-wave signal.
/// Temperature crosses the default 28 °C threshold, humidity crosses 70 %,
/// so the full state-machine and alert path is exercised without hardware.
class SimSensor final : public Sensor {
public:
  [[nodiscard]] std::expected<Reading, SensorError> read() const override {
    using namespace std::chrono;
    const auto now       = system_clock::now();
    const double elapsed = duration<double>(now.time_since_epoch()).count();

    // NOLINTBEGIN(readability-magic-numbers)
    constexpr double cyclePeriodSeconds = 6.0 * 60.0; // change to adjust oscillation speed

    const double phase = elapsed / cyclePeriodSeconds * (2.0 * std::numbers::pi); // radians: 0 to 2π over one cycle

    Reading reading;
    reading.timestamp   = now;
    reading.temperature = 24.0F + 8.0F * static_cast<float>(std::sin(phase));        // 16–32 °C
    reading.humidity    = 55.0F + 20.0F * static_cast<float>(std::sin(phase * 0.7)); // 35–75 %
    // NOLINTEND(readability-magic-numbers)
    return reading;
  }
};

} // namespace warden::sim

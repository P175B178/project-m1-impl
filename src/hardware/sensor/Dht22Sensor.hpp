#pragma once

#include "hardware/Reading.hpp"
#include "hardware/Sensor.hpp"

#include <chrono>

namespace warden::hardware {

/// Reads temperature and humidity from a DHT22 sensor via the Linux IIO kernel driver.
/// TODO: replace hardcoded values with real sysfs reads.
class Dht22Sensor final : public Sensor {
public:
  [[nodiscard]] std::expected<Reading, SensorError> read() const override {
    Reading reading;
    reading.timestamp   = std::chrono::system_clock::now();
    reading.temperature = 30.0F; // NOLINT(readability-magic-numbers) — above default 28 C threshold
    reading.humidity    = 75.0F; // NOLINT(readability-magic-numbers) — above default 70% threshold
    return reading;
  }
};

} // namespace warden::hardware

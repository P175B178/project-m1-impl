#pragma once

#include <chrono>
#include <cstddef>

/// All runtime configuration, loaded from a config file at startup.
// NOLINTBEGIN(readability-magic-numbers)
struct Config {
  // Sensor
  std::chrono::seconds readInterval{5};
  std::size_t averagingWindow{10};

  // Thresholds
  float temperatureThreshold{28.0F};
  float humidityThreshold{70.0F};

  // Sensor validation bounds — readings outside these are rejected as
  // implausible before entering the averaging buffer.
  float minTemperature{-40.0F};
  float maxTemperature{80.0F};
  float minHumidity{0.0F};
  float maxHumidity{100.0F};
};
// NOLINTEND(readability-magic-numbers)

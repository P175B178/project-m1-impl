#pragma once

#include <chrono>
#include <cstddef>

namespace warden {

/// All runtime configuration, loaded from a key=value file at startup.
struct Config {
    // Sensor
    std::chrono::seconds readInterval{5};
    std::size_t          averagingWindow{10};

    // Thresholds
    float temperatureThreshold{28.0f};
    float humidityThreshold{70.0f};

    // Sensor validation bounds — readings outside these are rejected as
    // implausible before entering the averaging buffer.
    float minTemperature{-40.0f};
    float maxTemperature{80.0f};
    float minHumidity{0.0f};
    float maxHumidity{100.0f};
};

} // namespace warden

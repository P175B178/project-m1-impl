#pragma once

#include "hardware/Reading.hpp"
#include "hardware/Sensor.hpp"

#include <chrono>
#include <cmath>
#include <numbers>

/// Simulated sensor that generates oscillating temperature and humidity.
/// Temperature swings 16-32 C, humidity swings 35-75%.
class SimSensor : public Sensor {
public:
    [[nodiscard]] std::expected<Reading, SensorError> read() const override {
        using namespace std::chrono;
        const auto now = system_clock::now();
        const duration<double> elapsed = now - start_;

        // Sine wave with ~6 minute period
        const double phase = elapsed.count() * 2.0 * std::numbers::pi / 360.0;

        Reading r;
        r.temperature = 24.0f + 8.0f * static_cast<float>(std::sin(phase));
        r.humidity = 55.0f + 20.0f * static_cast<float>(std::sin(phase * 1.3));
        r.timestamp = now;
        return r;
    }

private:
    std::chrono::system_clock::time_point start_ = std::chrono::system_clock::now();
};

#pragma once

#include "hardware/Reading.hpp"
#include "hardware/Sensor.hpp"

#include <chrono>
#include <cmath>

namespace warden::sim {

/// Simulated sensor that produces a slowly oscillating sine-wave signal.
/// Temperature crosses the default 28 °C threshold, humidity crosses 50 %,
/// so the full state-machine and alert path is exercised without hardware.
class SimSensor final : public Sensor {
public:
    [[nodiscard]] std::expected<Reading, SensorError> read() const override {
        using namespace std::chrono;
        const auto now   = system_clock::now();
        const double t   = duration<double>(now.time_since_epoch()).count();
        const double slow = t / 60.0; // one full cycle every ~6 minutes

        Reading r;
        r.timestamp   = now;
        r.temperature = 24.0F + 8.0F  * static_cast<float>(std::sin(slow));        // 16–32 °C
        r.humidity    = 55.0F + 20.0F * static_cast<float>(std::sin(slow * 0.7));   // 35–75 %
        return r;
    }
};

} // namespace warden::sim

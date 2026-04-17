#include "sim/SimSensor.hpp"

#include <spdlog/spdlog.h>

#include <thread>

int main() {
    spdlog::info("Warden starting...");

    SimSensor sensor;

    while (true) {
        auto result = sensor.read();
        if (result) {
            const auto& r = *result;
            spdlog::info("temp={:.1f} C, humidity={:.1f} %", r.temperature, r.humidity);
        } else {
            spdlog::warn("Sensor read failed");
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}

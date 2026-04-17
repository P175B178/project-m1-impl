#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <spdlog/spdlog.h>

#include <thread>

int main() {
    spdlog::info("Warden starting...");

    SimSensor sensor;
    StubLed led;
    StubBuzzer buzzer;

    const float tempThreshold = 28.0f;
    const float humThreshold = 70.0f;
    bool wasAlert = false;

    while (true) {
        auto result = sensor.read();
        if (!result) {
            spdlog::warn("Sensor read failed");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        const auto& r = *result;
        spdlog::info("temp={:.1f} C, humidity={:.1f} %", r.temperature, r.humidity);

        bool tempExceeded = r.temperature > tempThreshold;
        bool humExceeded = r.humidity > humThreshold;
        bool isAlert = tempExceeded && humExceeded;

        if (isAlert) {
            led.setMode(LedColor::Red, true);
            if (!wasAlert) {
                buzzer.shortBeep(3);
            }
        } else if (tempExceeded || humExceeded) {
            led.setMode(LedColor::Orange, false);
            if (wasAlert) {
                buzzer.longBeep(1);
            }
        } else {
            led.setMode(LedColor::Green, false);
            if (wasAlert) {
                buzzer.longBeep(1);
            }
        }

        wasAlert = isAlert;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}

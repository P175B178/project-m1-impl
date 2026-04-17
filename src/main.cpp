#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <cstdio>
#include <iostream>
#include <thread>

int main() {
    std::puts("Warden starting...");

    SimSensor sensor;
    StubLed led;
    StubBuzzer buzzer;

    const float tempThreshold = 28.0f;
    const float humThreshold = 70.0f;
    bool wasAlert = false;

    while (true) {
        auto result = sensor.read();
        if (!result) {
            std::cout << "Sensor read failed" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        const auto& r = *result;
        std::cout << "temp=" << r.temperature << " C, "
                  << "humidity=" << r.humidity << " %" << std::endl;

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

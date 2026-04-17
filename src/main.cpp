#include "sim/SimSensor.hpp"

#include <cstdio>
#include <iostream>
#include <thread>

int main() {
    std::puts("Warden starting...");

    SimSensor sensor;

    while (true) {
        auto result = sensor.read();
        if (result) {
            const auto& r = *result;
            std::cout << "temp=" << r.temperature << " C, "
                      << "humidity=" << r.humidity << " %" << std::endl;
        } else {
            std::cout << "Sensor read failed" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}

#include "core/StateMachine.hpp"
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <cstdio>
#include <iostream>
#include <thread>

void applyTransition(Led& led, Buzzer& buzzer, const StateTransition& t) {
    switch (t.to) {
    case State::Normal:
        led.setMode(LedColor::Green, false);
        break;
    case State::Warning:
        led.setMode(LedColor::Orange, false);
        break;
    case State::Alert:
        led.setMode(LedColor::Red, true);
        break;
    }

    if (t.to == State::Alert) {
        buzzer.shortBeep(3);
    } else if (t.from == State::Alert) {
        buzzer.longBeep(1);
    }
}

int main() {
    std::puts("Warden starting...");

    SimSensor sensor;
    StubLed led;
    StubBuzzer buzzer;
    StateMachine sm(28.0f, 70.0f);

    // Set initial LED state
    led.setMode(LedColor::Green, false);

    while (true) {
        auto result = sensor.read();
        if (!result) {
            std::cout << "Sensor read failed" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        const auto& r = *result;
        std::cout << "temp=" << r.temperature << " C, "
                  << "humidity=" << r.humidity << " %"
                  << " [" << stateToString(sm.currentState()) << "]" << std::endl;

        auto transition = sm.update({r.temperature, r.humidity});
        if (transition) {
            std::cout << "State: " << stateToString(transition->from)
                      << " -> " << stateToString(transition->to) << std::endl;
            applyTransition(led, buzzer, *transition);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}

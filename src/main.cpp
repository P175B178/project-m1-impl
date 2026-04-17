#include "core/StateMachine.hpp"
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <spdlog/spdlog.h>

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
    spdlog::info("Warden starting...");

    SimSensor sensor;
    StubLed led;
    StubBuzzer buzzer;
    StateMachine sm(28.0f, 70.0f);

    // Set initial LED state
    led.setMode(LedColor::Green, false);

    while (true) {
        auto result = sensor.read();
        if (!result) {
            spdlog::warn("Sensor read failed");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        const auto& r = *result;
        spdlog::info("temp={:.1f} C, humidity={:.1f} % [{}]",
                     r.temperature, r.humidity, stateToString(sm.currentState()));

        auto transition = sm.update({r.temperature, r.humidity});
        if (transition) {
            spdlog::info("State: {} -> {}",
                         stateToString(transition->from), stateToString(transition->to));
            applyTransition(led, buzzer, *transition);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}

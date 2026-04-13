#include "warden/WardenApp.hpp"

#include "warden/LedColor.hpp"
#include "warden/State.hpp"

#include <spdlog/spdlog.h>
#include <thread>

namespace warden {

WardenApp::WardenApp(Sensor& sensor, Led& led, Buzzer& buzzer, const Config& config)
    : sensor_{sensor}
    , led_{led}
    , buzzer_{buzzer}
    , config_{config}
    , stateMachine_{config.temperatureThreshold, config.humidityThreshold}
    , tempBuffer_{config.averagingWindow}
    , humBuffer_{config.averagingWindow}
{}

void WardenApp::run(const std::atomic<bool>& running) {
    led_.setMode(LedColor::Green);

    while (running) {
        const auto result = sensor_.read();

        if (!result) {
            spdlog::warn("Sensor read failed — skipping sample");
        } else {
            tempBuffer_.push(result->temperature);
            humBuffer_.push(result->humidity);

            const float avgTemp = tempBuffer_.average().value();
            const float avgHum  = humBuffer_.average().value();

            spdlog::info("temp={:.1f}°C (raw={:.1f})  hum={:.1f}% (raw={:.1f})  state={}",
                         avgTemp, result->temperature,
                         avgHum,  result->humidity,
                         stateToString(stateMachine_.currentState()));

            const auto transition = stateMachine_.update(avgTemp, avgHum);
            if (transition) {
                spdlog::warn("State change: {} → {}",
                             stateToString(transition->from),
                             stateToString(transition->to));
                applyTransition(*transition);
            }
        }

        std::this_thread::sleep_for(config_.readInterval);
    }

    led_.setOff();
}

void WardenApp::applyTransition(const StateTransition& t) {
    switch (t.to) {
        case State::Normal:  led_.setMode(LedColor::Green,  false); break;
        case State::Warning: led_.setMode(LedColor::Orange, false); break;
        case State::Alert:   led_.setMode(LedColor::Red,    true);  break;
    }

    if (t.to == State::Alert) {
        buzzer_.beep(3, 100, 100);  // three short beeps on entering Alert
    } else if (t.from == State::Alert) {
        buzzer_.beep(1, 500, 0);    // one long beep on leaving Alert
    }
}

} // namespace warden

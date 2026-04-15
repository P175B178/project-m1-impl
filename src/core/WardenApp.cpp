#include "WardenApp.hpp"

#include "State.hpp"
#include "hardware/LedColor.hpp"

#include <spdlog/spdlog.h>
#include <thread>

namespace warden {

WardenApp::WardenApp(Sensor &sensor, Led &led, Buzzer &buzzer, const Config &config)
    : sensor{&sensor}, led{&led}, buzzer{&buzzer}, config{&config},
      stateMachine{config.temperatureThreshold, config.humidityThreshold}, tempBuffer{config.averagingWindow},
      humBuffer{config.averagingWindow} {}

void WardenApp::run(const std::atomic<bool> &running) {
  led->setMode(LedColor::Green);

  while (running) {
    const auto result = sensor->read();

    if (!result) {
      spdlog::warn("Sensor read failed — skipping sample");
    } else {
      tempBuffer.push(result->temperature);
      humBuffer.push(result->humidity);

      // Safe: push() above guarantees count > 0, so average() always returns a value here.
      // NOLINTBEGIN(bugprone-unchecked-optional-access)
      const float avgTemp = tempBuffer.average().value();
      const float avgHum = humBuffer.average().value();
      // NOLINTEND(bugprone-unchecked-optional-access)

      spdlog::info("temp={:.1f}°C (raw={:.1f})  hum={:.1f}% (raw={:.1f})  state={}", avgTemp, result->temperature,
                   avgHum, result->humidity, stateToString(stateMachine.currentState()));

      const auto transition = stateMachine.update(avgTemp, avgHum);
      if (transition) {
        spdlog::warn("State change: {} → {}", stateToString(transition->from), stateToString(transition->to));
        applyTransition(*transition);
      }
    }

    std::this_thread::sleep_for(config->readInterval);
  }

  led->setOff();
}

void WardenApp::applyTransition(const StateTransition &t) {
  switch (t.to) {
  case State::Normal:
    led->setMode(LedColor::Green, false);
    break;
  case State::Warning:
    led->setMode(LedColor::Orange, false);
    break;
  case State::Alert:
    led->setMode(LedColor::Red, true);
    break;
  }

  if (t.to == State::Alert) {
    buzzer->beep(3, 100, 100); // three short beeps on entering Alert
  } else if (t.from == State::Alert) {
    buzzer->beep(1, 500, 0); // one long beep on leaving Alert
  }
}

} // namespace warden

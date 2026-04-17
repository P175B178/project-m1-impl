#include "WardenApp.hpp"

#include "Transition.hpp"

#include <spdlog/spdlog.h>

#include <thread>

namespace warden {

WardenApp::WardenApp(Sensor &sensor, Led &led, Buzzer &buzzer, const Config &config)
      : sensor{sensor}
      , led{led}
      , buzzer{buzzer}
      , config{config}
      , stateMachine{config.temperatureThreshold, config.humidityThreshold}
      , tempBuffer{config.averagingWindow}
      , humBuffer{config.averagingWindow} {}

void WardenApp::run() {
  led.setMode(LedColor::Green, false);

  while (true) {
    const auto result = sensor.read();

    if (!result) {
      spdlog::warn("Sensor read failed ({}) — skipping sample", sensorErrorToString(result.error()));
      std::this_thread::sleep_for(config.readInterval);
      continue;
    }

    const auto &reading = *result;

    // Validate plausible physical ranges
    bool tempOutOfRange = reading.temperature < config.minTemperature || reading.temperature > config.maxTemperature;
    bool humOutOfRange  = reading.humidity < config.minHumidity || reading.humidity > config.maxHumidity;
    if (tempOutOfRange || humOutOfRange) {
      spdlog::warn("Reading out of range (temp={:.1f}, hum={:.1f}) — skipping", reading.temperature, reading.humidity);
      std::this_thread::sleep_for(config.readInterval);
      continue;
    }

    tempBuffer.push(reading.temperature);
    humBuffer.push(reading.humidity);

    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    const float avgTemp = tempBuffer.average().value();
    const float avgHum  = humBuffer.average().value();
    // NOLINTEND(bugprone-unchecked-optional-access)

    spdlog::info("temp={:.1f} C (raw={:.1f})  hum={:.1f}% (raw={:.1f})  state={}", avgTemp, reading.temperature, avgHum,
                 reading.humidity, stateToString(stateMachine.currentState()));

    const auto transition = stateMachine.update({.temperature = avgTemp, .humidity = avgHum});
    if (transition) {
      spdlog::warn("State change: {} -> {}", stateToString(transition->from), stateToString(transition->to));
      applyTransition(led, buzzer, transition.value());
    }

    std::this_thread::sleep_for(config.readInterval);
  }
}

} // namespace warden

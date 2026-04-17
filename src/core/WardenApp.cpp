#include "WardenApp.hpp"

#include "Transition.hpp"

#include <spdlog/spdlog.h>

#include <condition_variable>
#include <mutex>

namespace warden {

WardenApp::WardenApp(Sensor &sensor, Led &led, Buzzer &buzzer, const Config &config)
      : sensor{sensor}
      , led{led}
      , buzzer{buzzer}
      , config{config}
      , stateMachine{config.temperatureThreshold, config.humidityThreshold}
      , tempBuffer{config.averagingWindow}
      , humBuffer{config.averagingWindow} {}

void WardenApp::run(std::stop_token stopToken) {
  led.setMode(LedColor::Green, false);

  std::mutex                  cvMutex;
  std::condition_variable_any cv;

  while (!stopToken.stop_requested()) {
    const auto result = sensor.read();

    if (!result) {
      spdlog::warn("Sensor read failed ({}) — skipping sample", sensorErrorToString(result.error()));
    } else {
      const auto &reading = *result;

      bool tempOutOfRange = reading.temperature < config.minTemperature || reading.temperature > config.maxTemperature;
      bool humOutOfRange  = reading.humidity < config.minHumidity || reading.humidity > config.maxHumidity;
      if (tempOutOfRange || humOutOfRange) {
        spdlog::warn("Reading out of range (temp={:.1f}, hum={:.1f}) — skipping", reading.temperature,
                     reading.humidity);
      } else {
        tempBuffer.push(reading.temperature);
        humBuffer.push(reading.humidity);

        // NOLINTBEGIN(bugprone-unchecked-optional-access)
        const float avgTemp = tempBuffer.average().value();
        const float avgHum  = humBuffer.average().value();
        // NOLINTEND(bugprone-unchecked-optional-access)

        spdlog::info("temp={:.1f} C (raw={:.1f})  hum={:.1f}% (raw={:.1f})  state={}", avgTemp, reading.temperature,
                     avgHum, reading.humidity, stateToString(stateMachine.currentState()));

        const auto transition = stateMachine.update({.temperature = avgTemp, .humidity = avgHum});
        if (transition) {
          spdlog::info("State change: {} -> {}", stateToString(transition->from), stateToString(transition->to));
          applyTransition(led, buzzer, transition.value());
        }
      }
    }

    std::unique_lock<std::mutex> lock(cvMutex);
    cv.wait_for(lock, stopToken, config.readInterval, [&] { return stopToken.stop_requested(); });
  }

  spdlog::info("Shutting down...");
  led.setOff();
  buzzer.setOff();
}

} // namespace warden

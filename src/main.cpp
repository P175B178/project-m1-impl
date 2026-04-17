#include "core/AveragingBuffer.hpp"
#include "core/ConfigLoader.hpp"
#include "core/StateMachine.hpp"
#include "core/Transition.hpp"
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <spdlog/spdlog.h>

#include <cstdio>
#include <cstdlib>
#include <thread>

int main() { // NOLINT(bugprone-exception-escape)
  std::puts("Warden starting...");

  const auto configResult = ConfigLoader::load("config/config.cfg");
  if (!configResult) {
    spdlog::error("Failed to load config: {}", configResult.error());
    return EXIT_FAILURE;
  }
  const auto &config = *configResult;

  spdlog::info("Temp threshold: {:.1f} C, humidity threshold: {:.1f}%", config.temperatureThreshold,
               config.humidityThreshold);
  spdlog::info("Read interval: {}s, averaging window: {} samples", config.readInterval.count(), config.averagingWindow);
  spdlog::info("Sensor validation: temp [{:.1f}, {:.1f}] C  hum [{:.1f}, {:.1f}]%", config.minTemperature,
               config.maxTemperature, config.minHumidity, config.maxHumidity);

  SimSensor sensor;
  StubLed led;
  StubBuzzer buzzer;
  StateMachine stateMachine(config.temperatureThreshold, config.humidityThreshold);
  AveragingBuffer<float> tempBuffer(config.averagingWindow);
  AveragingBuffer<float> humBuffer(config.averagingWindow);

  led.setMode(LedColor::Green, false);

  while (true) {
    auto result = sensor.read();
    if (!result) {
      spdlog::warn("Sensor read failed ({})", sensorErrorToString(result.error()));
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
    float avgTemp = *tempBuffer.average();
    float avgHum  = *humBuffer.average();
    // NOLINTEND(bugprone-unchecked-optional-access)

    spdlog::info("temp={:.1f} C (raw={:.1f})  hum={:.1f}% (raw={:.1f})  state={}", avgTemp, reading.temperature, avgHum,
                 reading.humidity, stateToString(stateMachine.currentState()));

    auto transition = stateMachine.update({.temperature = avgTemp, .humidity = avgHum});
    if (transition) {
      spdlog::warn("State change: {} -> {}", stateToString(transition->from), stateToString(transition->to));
      applyTransition(led, buzzer, *transition);
    }

    std::this_thread::sleep_for(config.readInterval);
  }

  return 0;
}

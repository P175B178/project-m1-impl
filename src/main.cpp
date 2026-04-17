#include "core/AveragingBuffer.hpp"
#include "core/StateMachine.hpp"
#include "core/Transition.hpp"
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <spdlog/spdlog.h>

#include <cstdio>
#include <thread>

int main() { // NOLINT(bugprone-exception-escape)
  std::puts("Warden starting...");

  SimSensor sensor;
  StubLed led;
  StubBuzzer buzzer;
  StateMachine stateMachine(28.0F, 70.0F);               // NOLINT(readability-magic-numbers)
  AveragingBuffer<float> tempBuffer(10);                 // NOLINT(readability-magic-numbers)
  AveragingBuffer<float> humBuffer(10);                  // NOLINT(readability-magic-numbers)
  constexpr auto readInterval = std::chrono::seconds(5); // NOLINT(readability-magic-numbers)

  led.setMode(LedColor::Green, false);

  while (true) {
    auto result = sensor.read();
    if (!result) {
      spdlog::warn("Sensor read failed");
      std::this_thread::sleep_for(readInterval);
      continue;
    }

    const auto &reading = *result;
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

    std::this_thread::sleep_for(readInterval);
  }

  return 0;
}

#include "core/AveragingBuffer.hpp"
#include "core/StateMachine.hpp"
#include "core/Transition.hpp"
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <cstdio>
#include <iostream>
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
      std::cout << "Sensor read failed\n";
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

    std::cout << "temp=" << avgTemp << " C (raw=" << reading.temperature << "), "
              << "humidity=" << avgHum << " % (raw=" << reading.humidity << ")"
              << " [" << stateToString(stateMachine.currentState()) << "]\n";

    auto transition = stateMachine.update({.temperature = avgTemp, .humidity = avgHum});
    if (transition) {
      std::cout << "State: " << stateToString(transition->from) << " -> " << stateToString(transition->to) << '\n';
      applyTransition(led, buzzer, *transition);
    }

    std::this_thread::sleep_for(readInterval);
  }

  return 0;
}

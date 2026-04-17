#include "core/AveragingBuffer.hpp"
#include "core/StateMachine.hpp"
#include "core/Transition.hpp"
#include "sim/SimSensor.hpp"
#include "sim/StubBuzzer.hpp"
#include "sim/StubLed.hpp"

#include <spdlog/spdlog.h>

#include <condition_variable>
#include <csignal>
#include <mutex>
#include <stop_token>

namespace {
std::stop_source g_stopSource; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void handleSignal(int /*sig*/) {
  g_stopSource.request_stop();
}
} // namespace

int main() { // NOLINT(bugprone-exception-escape)
  spdlog::info("Warden starting...");

  std::signal(SIGINT, handleSignal);
  std::signal(SIGTERM, handleSignal);

  SimSensor sensor;
  StubLed led;
  StubBuzzer buzzer;
  StateMachine sm(28.0f, 70.0f);
  AveragingBuffer<float> tempBuffer(10);
  AveragingBuffer<float> humBuffer(10);

  // NOLINTBEGIN(readability-magic-numbers)
  constexpr float minTemperature = -40.0F;
  constexpr float maxTemperature = 80.0F;
  constexpr float minHumidity    = 0.0F;
  constexpr float maxHumidity    = 100.0F;
  // NOLINTEND(readability-magic-numbers)

  led.setMode(LedColor::Green, false);

  std::mutex cvMutex;
  std::condition_variable_any cv;
  const std::stop_token stopToken = g_stopSource.get_token();

  while (!stopToken.stop_requested()) {
    auto result = sensor.read();
    if (!result) {
      spdlog::warn("Sensor read failed ({})", sensorErrorToString(result.error()));
    } else {
      const auto &reading = *result;

      if (reading.temperature < minTemperature || reading.temperature > maxTemperature ||
          reading.humidity < minHumidity || reading.humidity > maxHumidity) {
        spdlog::warn("Reading out of range (temp={:.1f}, hum={:.1f}) — skipping", reading.temperature,
                     reading.humidity);
      } else {
        tempBuffer.push(reading.temperature);
        humBuffer.push(reading.humidity);

        // NOLINTBEGIN(bugprone-unchecked-optional-access)
        float avgTemp = *tempBuffer.average();
        float avgHum  = *humBuffer.average();
        // NOLINTEND(bugprone-unchecked-optional-access)

        spdlog::info("temp={:.1f} C (raw={:.1f})  hum={:.1f}% (raw={:.1f})  state={}", avgTemp, reading.temperature,
                     avgHum, reading.humidity, stateToString(sm.currentState()));

        auto transition = sm.update({.temperature = avgTemp, .humidity = avgHum});
        if (transition) {
          spdlog::info("State change: {} -> {}", stateToString(transition->from), stateToString(transition->to));
          applyTransition(led, buzzer, *transition);
        }
      }
    }

    std::unique_lock<std::mutex> lock(cvMutex);
    cv.wait_for(lock, stopToken, std::chrono::seconds(5), [&] { return stopToken.stop_requested(); });
  }

  spdlog::info("Shutting down...");
  led.setOff();
  buzzer.setOff();

  return 0;
}

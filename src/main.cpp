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

  // Set initial LED state
  led.setMode(LedColor::Green, false);

  std::mutex cvMutex;
  std::condition_variable_any cv;
  const std::stop_token stopToken = g_stopSource.get_token();

  while (!stopToken.stop_requested()) {
    auto result = sensor.read();
    if (result) {
      const auto &r = *result;
      tempBuffer.push(r.temperature);
      humBuffer.push(r.humidity);

      float avgTemp = *tempBuffer.average();
      float avgHum  = *humBuffer.average();

      spdlog::info("temp={:.1f} C (raw={:.1f}), humidity={:.1f} % (raw={:.1f}) [{}]", avgTemp, r.temperature, avgHum,
                   r.humidity, stateToString(sm.currentState()));

      auto transition = sm.update({avgTemp, avgHum});
      if (transition) {
        spdlog::info("State: {} -> {}", stateToString(transition->from), stateToString(transition->to));
        applyTransition(led, buzzer, *transition);
      }
    } else {
      spdlog::warn("Sensor read failed");
    }

    std::unique_lock<std::mutex> lock(cvMutex);
    cv.wait_for(lock, stopToken, std::chrono::seconds(5), [&] { return stopToken.stop_requested(); });
  }

  spdlog::info("Shutting down...");
  led.setOff();
  buzzer.setOff();

  return 0;
}

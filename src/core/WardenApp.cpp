#include "WardenApp.hpp"

#include "State.hpp"
#include "hardware/LedColor.hpp"

#include <condition_variable>
#include <mutex>
#include <spdlog/spdlog.h>

namespace warden {

WardenApp::WardenApp(Sensor &sensor, Led &led, Buzzer &buzzer, const Config &config)
      : sensor{sensor}
      , led{led}
      , buzzer{buzzer}
      , config{config}
      , stateMachine{config.temperatureThreshold, config.humidityThreshold}
      , tempBuffer{config.averagingWindow}
      , humBuffer{config.averagingWindow} {}

void WardenApp::run(std::stop_token stopToken) { // NOLINT(performance-unnecessary-value-param) -- stop_token is
                                                 // designed to be passed by value
  led.setMode(LedColor::Green, false);

  std::jthread sensorThread{[this](std::stop_token token) { sensorLoop(std::move(token)); }};

  // When the caller requests stop, propagate it to the sensor thread and
  // the queue so the control loop unblocks and exits cleanly.
  std::stop_callback stopOnRequest{stopToken, [&] {
                                     sensorThread.request_stop();
                                     readingQueue.stop();
                                   }};

  while (auto reading = readingQueue.pop()) {
    processReading(reading.value());
  }

  led.setOff();
}

void WardenApp::sensorLoop(std::stop_token stopToken) { // NOLINT(performance-unnecessary-value-param) -- stop_token is
                                                        // designed to be passed by value
  std::mutex sleepMutex;
  std::condition_variable_any sleepCv;

  while (!stopToken.stop_requested()) {
    const auto result = sensor.read();

    if (!result) {
      spdlog::warn("Sensor read failed — skipping sample");
    } else {
      readingQueue.push(*result);
    }

    // Sleep after every iteration, including failed reads — this rate-limits
    // the sensor regardless of error state and avoids hammering broken hardware.
    std::unique_lock lock{sleepMutex};
    sleepCv.wait_for(lock, stopToken, config.readInterval, [] { return false; });
  }
}

void WardenApp::processReading(const Reading &reading) {
  tempBuffer.push(reading.temperature);
  humBuffer.push(reading.humidity);

  // Safe: push() above guarantees count > 0, so average() always returns a value here.
  // NOLINTBEGIN(bugprone-unchecked-optional-access)
  const float avgTemp = tempBuffer.average().value();
  const float avgHum  = humBuffer.average().value();
  // NOLINTEND(bugprone-unchecked-optional-access)

  spdlog::info("temp={:.1f}°C (raw={:.1f})  hum={:.1f}% (raw={:.1f})  state={}", avgTemp, reading.temperature, avgHum,
               reading.humidity, stateToString(stateMachine.currentState()));

  const auto transition = stateMachine.update({.temperature = avgTemp, .humidity = avgHum});
  if (transition) {
    spdlog::warn("State change: {} → {}", stateToString(transition->from), stateToString(transition->to));
    applyTransition(*transition);
  }
}

void WardenApp::applyTransition(const StateTransition &transition) {
  switch (transition.to) {
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

  if (transition.to == State::Alert) {
    buzzer.shortBeep(3); // NOLINT(readability-magic-numbers)
  } else if (transition.from == State::Alert) {
    buzzer.longBeep(1); // NOLINT(readability-magic-numbers)
  }
}

} // namespace warden

#pragma once

#include "AveragingBuffer.hpp"
#include "Config.hpp"
#include "StateMachine.hpp"
#include "hardware/Buzzer.hpp"
#include "hardware/Led.hpp"
#include "hardware/Sensor.hpp"

#include <atomic>

namespace warden {

/// Owns the monitoring loop: reads the sensor, maintains averaging buffers,
/// drives the state machine, and reacts to state transitions via LED and buzzer.
///
/// Hardware dependencies are injected so the class can be used with any
/// conforming implementation.
class WardenApp {
public:
  WardenApp(Sensor &sensor, Led &led, Buzzer &buzzer, const Config &config);

  /// Run the monitoring loop until `running` becomes false.
  /// Sets the LED to green on entry, turns it off on exit.
  void run(const std::atomic<bool> &running);

  /// Apply the LED and buzzer reaction for a state transition.
  void applyTransition(const StateTransition &transition);

private:
  Sensor *sensor;
  Led *led;
  Buzzer *buzzer;
  const Config *config;

  StateMachine stateMachine;
  AveragingBuffer<float> tempBuffer;
  AveragingBuffer<float> humBuffer;
};

} // namespace warden

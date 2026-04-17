#pragma once

#include "AveragingBuffer.hpp"
#include "Config.hpp"
#include "StateMachine.hpp"
#include "hardware/Buzzer.hpp"
#include "hardware/Led.hpp"
#include "hardware/Sensor.hpp"

#include <stop_token>

namespace warden {

/// Owns the monitoring loop: reads the sensor, maintains averaging buffers,
/// drives the state machine, and reacts to state transitions via LED and buzzer.
///
/// Hardware dependencies are injected so the class can be used with any
/// conforming implementation.
class WardenApp {
public:
  WardenApp(Sensor &sensor, Led &led, Buzzer &buzzer, const Config &config);

  ~WardenApp()                            = default;
  WardenApp(const WardenApp &)            = delete;
  WardenApp &operator=(const WardenApp &) = delete;
  WardenApp(WardenApp &&)                 = delete;
  WardenApp &operator=(WardenApp &&)      = delete;

  /// Run the monitoring loop until the stop token is requested.
  void run(std::stop_token stopToken);

private:
  Sensor &sensor;
  Led &led;
  Buzzer &buzzer;
  const Config &config;

  StateMachine stateMachine;
  AveragingBuffer<float> tempBuffer;
  AveragingBuffer<float> humBuffer;
};

} // namespace warden

#pragma once

#include "AveragingBuffer.hpp"
#include "BlockingQueue.hpp"
#include "Config.hpp"
#include "StateMachine.hpp"
#include "hardware/Buzzer.hpp"
#include "hardware/Led.hpp"
#include "hardware/Reading.hpp"
#include "hardware/Sensor.hpp"

#include <stop_token>

namespace warden {

/// Owns the monitoring loop: reads the sensor, maintains averaging buffers,
/// drives the state machine, and reacts to state transitions via LED and buzzer.
///
/// Hardware dependencies are injected so the class can be used with any
/// conforming implementation.
///
/// Threading model:
///   - sensorLoop() runs on a dedicated thread: reads the sensor at
///     config.readInterval and pushes raw readings onto the blocking queue.
///   - run() consumes from the queue on the caller's thread: averages readings,
///     updates the state machine, and applies transitions.
class WardenApp {
public:
  WardenApp(Sensor &sensor, Led &led, Buzzer &buzzer, const Config &config);

  ~WardenApp()                            = default;
  WardenApp(const WardenApp &)            = delete;
  WardenApp &operator=(const WardenApp &) = delete;
  WardenApp(WardenApp &&)                 = delete;
  WardenApp &operator=(WardenApp &&)      = delete;

  /// Run the monitoring loop until the stop token is signalled.
  /// Sets the LED to green on entry, turns it off on exit.
  void run(std::stop_token stopToken);

  /// Apply the LED and buzzer reaction for a state transition.
  void applyTransition(const StateTransition &transition);

private:
  Sensor &sensor;
  Led &led;
  Buzzer &buzzer;
  const Config &config;

  StateMachine stateMachine;
  AveragingBuffer<float> tempBuffer;
  AveragingBuffer<float> humBuffer;

  BlockingQueue<Reading> readingQueue;

  /// Producer: reads the sensor periodically and pushes readings onto
  /// readingQueue.  Runs on a std::jthread; stop is requested via stopToken.
  void sensorLoop(std::stop_token stopToken);

  /// Consumer: averages a raw reading, updates the state machine, and
  /// applies any resulting transition.
  void processReading(const Reading &reading);
};

} // namespace warden

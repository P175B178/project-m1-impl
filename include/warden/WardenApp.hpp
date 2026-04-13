#pragma once

#include "warden/AveragingBuffer.hpp"
#include "warden/Buzzer.hpp"
#include "warden/Config.hpp"
#include "warden/Led.hpp"
#include "warden/Sensor.hpp"
#include "warden/StateMachine.hpp"

#include <atomic>

namespace warden {

/// Owns the monitoring loop: reads the sensor, maintains averaging buffers,
/// drives the state machine, and reacts to state transitions via LED and buzzer.
///
/// Hardware dependencies are injected so the class can be used with any
/// conforming implementation.
class WardenApp {
public:
    WardenApp(Sensor& sensor, Led& led, Buzzer& buzzer, const Config& config);

    /// Run the monitoring loop until `running` becomes false.
    /// Sets the LED to green on entry, turns it off on exit.
    void run(const std::atomic<bool>& running);

    /// Apply the LED and buzzer reaction for a state transition.
    void applyTransition(const StateTransition& transition);

private:
    Sensor&       sensor_;
    Led&          led_;
    Buzzer&       buzzer_;
    const Config& config_;

    StateMachine           stateMachine_;
    AveragingBuffer<float> tempBuffer_;
    AveragingBuffer<float> humBuffer_;
};

} // namespace warden

#pragma once

#include "State.hpp"
#include "StateMachine.hpp"
#include "hardware/Buzzer.hpp"
#include "hardware/Led.hpp"
#include "hardware/LedColor.hpp"

namespace warden {

/// Apply the LED and buzzer reaction for a state transition.
/// Called by WardenApp whenever the state machine produces a new transition.
inline void applyTransition(Led &led, Buzzer &buzzer, const StateTransition &transition) {
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

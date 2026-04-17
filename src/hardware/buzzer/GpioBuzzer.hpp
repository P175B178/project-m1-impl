#pragma once

#include "hardware/Buzzer.hpp"

namespace warden::hardware {

/// Drives a passive buzzer via GPIO square wave.
/// TODO: replace with real libgpiod driver.
class GpioBuzzer final : public Buzzer {
public:
  void shortBeep(unsigned int /*count*/) override {}
  void longBeep(unsigned int /*count*/) override {}
  void setOff() override {}
};

} // namespace warden::hardware

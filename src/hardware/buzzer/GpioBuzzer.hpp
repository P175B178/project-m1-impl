#pragma once

#include "GpioPin.hpp"
#include "hardware/Buzzer.hpp"

#include <chrono>
#include <string>

namespace warden::hardware {

/// Passive buzzer driven by a single GPIO output pin.
/// Emits a tone by toggling the pin at a fixed frequency.
class GpioBuzzer : public warden::Buzzer {
public:
  /// @param chipPath  GPIO chip device path, e.g. "/dev/gpiochip4"
  explicit GpioBuzzer(const std::string &chipPath);

  void shortBeep(int count) override;
  void longBeep(int count) override;

private:
  void emitTone(std::chrono::milliseconds duration);

  GpioPin pin;
};

} // namespace warden::hardware

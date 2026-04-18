#pragma once

#include "GpioPin.hpp"
#include "hardware/Buzzer.hpp"

#include <chrono>
#include <string>

namespace warden::hardware {

/// Passive buzzer driven by a single GPIO output pin.
/// Emits a tone by toggling the pin at a fixed frequency.
///
/// Multi-beep sequences are not yet implemented — short/long beeps emit a
/// single tone regardless of the requested count.
class GpioBuzzer : public warden::Buzzer {
public:
  explicit GpioBuzzer(const std::string &chipPath = "/dev/gpiochip4");
  ~GpioBuzzer() override;

  GpioBuzzer(const GpioBuzzer &)            = delete;
  GpioBuzzer &operator=(const GpioBuzzer &) = delete;
  GpioBuzzer(GpioBuzzer &&)                 = delete;
  GpioBuzzer &operator=(GpioBuzzer &&)      = delete;

  void shortBeep(unsigned int count) override;
  void longBeep(unsigned int count) override;
  void setOff() override;

private:
  void emitTone(std::chrono::milliseconds duration);

  GpioPin pin;
};

} // namespace warden::hardware

#pragma once

#include "GpioPin.hpp"
#include "hardware/Buzzer.hpp"

#include <chrono>
#include <stop_token>
#include <string>
#include <thread>

namespace warden::hardware {

/// Passive buzzer driven by a single GPIO output pin.
/// Emits a tone by toggling the pin at a fixed frequency.
///
/// shortBeep() and longBeep() are non-blocking — they start a background
/// thread and return immediately. A new call cancels any in-progress beep.
///
/// Non-copyable and non-movable because the instance owns exclusive control
/// of a hardware pin and a background thread.
class GpioBuzzer : public warden::Buzzer {
public:
  /// @param chipPath  GPIO chip device path, e.g. "/dev/gpiochip4"
  explicit GpioBuzzer(const std::string &chipPath);
  ~GpioBuzzer() override;

  GpioBuzzer(const GpioBuzzer &)            = delete;
  GpioBuzzer &operator=(const GpioBuzzer &) = delete;
  GpioBuzzer(GpioBuzzer &&)                 = delete;
  GpioBuzzer &operator=(GpioBuzzer &&)      = delete;

  void shortBeep(int count) override;
  void longBeep(int count) override;

private:
  void startBeeping(std::chrono::milliseconds duration, int count);
  void stopBeeping();
  void emitTone(std::chrono::milliseconds duration, std::stop_token stop);

  GpioPin pin;
  std::jthread beepThread;
};

} // namespace warden::hardware

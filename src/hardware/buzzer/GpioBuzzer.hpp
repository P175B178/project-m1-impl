#pragma once

#include "GpioPin.hpp"
#include "hardware/Buzzer.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

namespace warden::hardware {

/// Passive buzzer driven by a single GPIO output pin.
/// Emits a tone by toggling the pin at a fixed frequency.
///
/// shortBeep() and longBeep() are non-blocking — they start a background
/// thread and return immediately. A new call cancels any in-progress beep.
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
  void startBeeping(std::chrono::milliseconds duration, unsigned int count);
  void stopBeeping();
  void emitTone(std::chrono::milliseconds duration);

  GpioPin pin;
  std::atomic<bool> beepRunning{false};
  std::thread beepThread;
};

} // namespace warden::hardware

#pragma once

#include <gpiod.h>
#include <string>

namespace warden::hardware {

/// RAII wrapper around a single libgpiod v2 output line.
///
/// The chip path on Raspberry Pi 5 is typically /dev/gpiochip4 (RP1).
/// Pin numbers use BCM numbering.
class GpioPin {
public:
  /// @param chipPath   e.g. "/dev/gpiochip4"
  /// @param lineOffset BCM pin number
  /// @param consumer   label shown in gpioinfo
  GpioPin(const std::string &chipPath, unsigned int lineOffset, const std::string &consumer);
  ~GpioPin();

  GpioPin(const GpioPin &)            = delete;
  GpioPin &operator=(const GpioPin &) = delete;
  GpioPin(GpioPin &&)                 = delete;
  GpioPin &operator=(GpioPin &&)      = delete;

  void setHigh();
  void setLow();

private:
  gpiod_chip *chip{nullptr};
  gpiod_line_request *request{nullptr};
  unsigned int offset{0};
};

} // namespace warden::hardware

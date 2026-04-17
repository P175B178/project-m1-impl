#pragma once

#include "hardware/Led.hpp"

namespace warden::hardware {

/// Controls the Pi's built-in ACT and PWR LEDs to signal state.
/// TODO: replace with real sysfs LED driver.
class StatusLed final : public Led {
public:
  void setMode(LedColor /*color*/, bool /*blink*/) override {}
  void setOff() override {}
};

} // namespace warden::hardware

#pragma once

#include "SysfsLed.hpp"
#include "hardware/Led.hpp"

namespace warden::hardware {

/// Status LED driver composing the Pi's built-in ACT (green) and PWR (red)
/// LEDs to produce the requested visual mode.
///
/// Colour mapping (ACT=green, PWR=red):
///   Green  -> ACT on,  PWR off
///   Orange -> ACT on,  PWR on
///   Red    -> ACT off, PWR on
///
/// Blinking not yet implemented — blink requests are shown as solid colour.
class StatusLed : public warden::Led {
public:
  StatusLed();
  ~StatusLed() override;

  StatusLed(const StatusLed &)            = delete;
  StatusLed &operator=(const StatusLed &) = delete;
  StatusLed(StatusLed &&)                 = delete;
  StatusLed &operator=(StatusLed &&)      = delete;

  void setMode(warden::LedColor color, bool blink) override;
  void setOff() override;

private:
  void applyColor(warden::LedColor color);
  void clearColor();

  SysfsLed act{"ACT", /*inverted=*/true};  // green
  SysfsLed pwr{"PWR", /*inverted=*/false}; // red
};

} // namespace warden::hardware

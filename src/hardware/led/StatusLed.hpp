#pragma once

#include "SysfsLed.hpp"
#include "hardware/Led.hpp"

#include <atomic>
#include <thread>

namespace warden::hardware {

/// Status LED driver composing the Pi's built-in ACT (green) and PWR (red)
/// LEDs to produce the requested visual mode.
///
/// Colour mapping (ACT=green, PWR=red):
///   Green  -> ACT on,  PWR off
///   Orange -> ACT on,  PWR on
///   Red    -> ACT off, PWR on
///
/// When blink=true, a background thread toggles all active pins at 1 Hz.
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
  void startBlinking(warden::LedColor color);
  void stopBlinking();

  SysfsLed act{"ACT", /*inverted=*/true};  // green
  SysfsLed pwr{"PWR", /*inverted=*/false}; // red

  std::atomic<bool> blinkRunning{false};
  std::thread blinkThread;
};

} // namespace warden::hardware

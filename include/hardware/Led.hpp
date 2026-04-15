#pragma once

#include "LedColor.hpp"

namespace warden {

/// Abstract LED interface.
///
/// Color and blink mode are orthogonal — any color can be steady or blinking.
/// setMode() and setOff() are non-blocking and cancel any in-progress blink.
class Led {
public:
  Led()                       = default;
  virtual ~Led()              = default;
  Led(const Led &)            = delete;
  Led &operator=(const Led &) = delete;
  Led(Led &&)                 = delete;
  Led &operator=(Led &&)      = delete;

  /// Set the active color and blink mode. Non-blocking — returns immediately.
  /// Cancels any blink currently in progress before applying the new mode.
  /// @param color  The color to display.
  /// @param blink  If true, the LED blinks at a fixed frequency; otherwise steady.
  virtual void setMode(LedColor color, bool blink) = 0;

  /// Turn off all LEDs immediately. Cancels any blink currently in progress.
  virtual void setOff() = 0;
};

} // namespace warden

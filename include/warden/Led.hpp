#pragma once

#include "LedColor.hpp"

namespace warden {

/// Abstract LED interface.
///
/// Color and blink are separate parameters because they are orthogonal
/// concerns — any color can in principle blink or be steady.
class Led {
public:
  virtual ~Led() = default;

  virtual void setMode(LedColor color, bool blink = false) = 0;

  /// Turn the LED off unconditionally (used on shutdown).
  virtual void setOff() = 0;
};

} // namespace warden

#pragma once

#include "LedColor.hpp"

namespace warden {

/// Abstract LED interface.
///
/// Color and blink are separate parameters because they are orthogonal
/// concerns — any color can in principle blink or be steady.
class Led {
public:
  Led() = default;
  virtual ~Led() = default;
  Led(const Led &) = delete;
  Led &operator=(const Led &) = delete;
  Led(Led &&) = delete;
  Led &operator=(Led &&) = delete;

  virtual void setMode(LedColor color, bool blink = false) = 0;

  /// Turn the LED off unconditionally (used on shutdown).
  virtual void setOff() = 0;
};

} // namespace warden

#pragma once

#include "LedColor.hpp"

/// Abstract LED interface.
class Led {
public:
  Led()                       = default;
  virtual ~Led()              = default;
  Led(const Led &)            = delete;
  Led &operator=(const Led &) = delete;
  Led(Led &&)                 = delete;
  Led &operator=(Led &&)      = delete;

  virtual void setMode(LedColor color, bool blink) = 0;
  virtual void setOff()                            = 0;
};

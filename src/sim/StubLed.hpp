#pragma once

#include "hardware/Led.hpp"
#include "hardware/LedColor.hpp"

#include <iostream>

/// Simulated LED — prints state changes to stdout.
class StubLed final : public Led {
public:
  void setMode(LedColor color, bool blink) override {
    std::cout << "[LED] " << toString(color) << (blink ? " (blinking)" : "") << '\n';
  }

  void setOff() override { std::cout << "[LED] OFF\n"; }

private:
  static const char *toString(LedColor color) {
    switch (color) {
    case LedColor::Green:
      return "GREEN";
    case LedColor::Orange:
      return "ORANGE";
    case LedColor::Red:
      return "RED";
    default:
      return "UNKNOWN";
    }
  }
};

#pragma once

#include "hardware/Led.hpp"

#include <iostream>

/// Simulated LED — prints state changes to stdout.
class StubLed : public Led {
public:
  void setMode(LedColor color, bool blink) override {
    const char *name = "?";
    switch (color) {
    case LedColor::Green:
      name = "GREEN";
      break;
    case LedColor::Orange:
      name = "ORANGE";
      break;
    case LedColor::Red:
      name = "RED";
      break;
    }
    std::cout << "[LED] " << name << (blink ? " (blinking)" : "") << std::endl;
  }

  void setOff() override { std::cout << "[LED] OFF" << std::endl; }
};

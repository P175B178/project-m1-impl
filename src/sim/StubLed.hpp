#pragma once

#include "hardware/Led.hpp"
#include "hardware/LedColor.hpp"

#include <spdlog/spdlog.h>

namespace warden::sim {

/// No-op LED that logs state changes to the console.
class StubLed final : public Led {
public:
  void setMode(LedColor color, bool blink) override {
    spdlog::info("[LED] {} {}", toString(color), blink ? "blinking" : "");
  }

  void setOff() override { spdlog::info("[LED] off"); }

private:
  static const char *toString(LedColor color) {
    switch (color) {
    case LedColor::Green:
      return "green";
    case LedColor::Orange:
      return "orange";
    case LedColor::Red:
      return "red";
    default:
      return "unknown";
    }
  }
};

} // namespace warden::sim

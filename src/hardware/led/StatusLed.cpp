#include "StatusLed.hpp"

#include <spdlog/spdlog.h>

namespace warden::hardware {

StatusLed::StatusLed() = default;

StatusLed::~StatusLed() {
  clearColor();
}

void StatusLed::clearColor() {
  act.setOff();
  pwr.setOff();
}

void StatusLed::applyColor(warden::LedColor color) {
  switch (color) {
  case warden::LedColor::Green:
    act.setOn();
    pwr.setOff();
    break;
  case warden::LedColor::Orange:
    act.setOn();
    pwr.setOn();
    break;
  case warden::LedColor::Red:
    act.setOff();
    pwr.setOn();
    break;
  }
}

void StatusLed::setMode(warden::LedColor color, bool blink) {
  spdlog::debug("StatusLed: setMode color={} blink={} (blinking not yet implemented)", static_cast<int>(color), blink);
  applyColor(color);
}

void StatusLed::setOff() {
  clearColor();
}

} // namespace warden::hardware

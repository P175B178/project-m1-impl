#include "StatusLed.hpp"

#include <chrono>
#include <spdlog/spdlog.h>

using namespace std::chrono_literals;

namespace warden::hardware {

StatusLed::StatusLed() = default;

StatusLed::~StatusLed() {
  stopBlinking();
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
  spdlog::debug("StatusLed: setMode color={} blink={}", static_cast<int>(color), blink);
  stopBlinking();
  if (blink) {
    startBlinking(color);
  } else {
    applyColor(color);
  }
}

void StatusLed::setOff() {
  stopBlinking();
  clearColor();
}

void StatusLed::startBlinking(warden::LedColor color) {
  blinkRunning = true;
  blinkThread  = std::thread{[this, color] {
    bool on = true;
    spdlog::debug("blink thread started");

    while (blinkRunning) {
      if (on) {
        applyColor(color);
      } else {
        clearColor();
      }
      on = !on;
      std::this_thread::sleep_for(500ms); // NOLINT(readability-magic-numbers)
    }

    clearColor();
    spdlog::debug("blink thread stopped");
  }};
}

void StatusLed::stopBlinking() {
  if (blinkThread.joinable()) {
    spdlog::debug("StatusLed: stopping blink thread");
    blinkRunning = false;
    blinkThread.join();
  }
}

} // namespace warden::hardware

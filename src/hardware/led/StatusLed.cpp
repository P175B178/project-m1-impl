#include "StatusLed.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stop_token>
#include <thread>

#include <spdlog/spdlog.h>

using namespace std::chrono_literals;

namespace {
constexpr auto blinkPeriod = 1000ms;
}

namespace warden::hardware {

StatusLed::StatusLed() = default;

StatusLed::~StatusLed() {
  stopBlinking();
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
  // Stop any running blink thread before touching the pins to avoid
  // a race between the thread and the new color assignment.
  spdlog::debug("StatusLed: setMode color={} blink={}", static_cast<int>(color), blink);
  stopBlinking();
  applyColor(color);
  if (blink) {
    startBlinking(color);
  }
}

void StatusLed::setOff() {
  stopBlinking();
  act.setOff();
  pwr.setOff();
}

void StatusLed::startBlinking(warden::LedColor color) {
  blinkThread = std::jthread{[this, color](std::stop_token stop) {
    std::mutex sleepMutex;
    std::condition_variable_any sleepCv;
    bool on = true;
    spdlog::debug("blink thread started");
    while (!stop.stop_requested()) {
      spdlog::debug("blink: {}", on ? "on" : "off");
      if (on) {
        applyColor(color);
      } else {
        act.setOff();
        pwr.setOff();
      }
      std::unique_lock lock{sleepMutex};
      sleepCv.wait_for(lock, stop, blinkPeriod / 2, [] { return false; });
      on = !on;
    }
    act.setOff();
    pwr.setOff();
    spdlog::debug("blink thread stopped");
  }};
}

void StatusLed::stopBlinking() {
  if (blinkThread.joinable()) {
    spdlog::debug("StatusLed: stopping blink thread");
    blinkThread.request_stop();
    blinkThread.join();
  }
}

} // namespace warden::hardware

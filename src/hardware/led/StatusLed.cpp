#include "StatusLed.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
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
  act_.setOff();
  pwr_.setOff();
}

void StatusLed::setMode(warden::LedColor color, bool blink) {
  // Stop any running blink thread before touching the pins to avoid
  // a race between the thread and the new color assignment.
  spdlog::debug("StatusLed: setMode color={} blink={}", static_cast<int>(color), blink);
  stopBlinking();

  switch (color) {
  case warden::LedColor::Green:
    act_.setOn();
    pwr_.setOff();
    break;
  case warden::LedColor::Orange:
    act_.setOn();
    pwr_.setOn();
    break;
  case warden::LedColor::Red:
    act_.setOff();
    pwr_.setOn();
    break;
  }

  if (blink) {
    startBlinking();
  }
}

void StatusLed::setOff() {
  stopBlinking();
  act_.setOff();
  pwr_.setOff();
}

void StatusLed::startBlinking() {
  // condition_variable_any + stop_token lets the thread wake immediately
  // on shutdown rather than sleeping for up to half a period.
  blinkThread_ = std::jthread{[this](std::stop_token stop) {
    spdlog::debug("blink thread started");
    while (!stop.stop_requested()) {
      spdlog::debug("blink: on");
      pwr_.setOn();
      std::this_thread::sleep_for(blinkPeriod / 2);
      if (stop.stop_requested()) {
        break;
      }
      spdlog::debug("blink: off");
      pwr_.setOff();
      std::this_thread::sleep_for(blinkPeriod / 2);
    }
    pwr_.setOff();
    spdlog::debug("blink thread stopped");
  }};
}

void StatusLed::stopBlinking() {
  if (blinkThread_.joinable()) {
    spdlog::debug("StatusLed: stopping blink thread");
    blinkThread_.request_stop();
    blinkThread_.join();
  }
}

} // namespace warden::hardware

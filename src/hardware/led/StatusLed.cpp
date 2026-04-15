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

void StatusLed::setMode(warden::LedColor color, bool blink) {
  // Stop any running blink thread before touching the pins to avoid
  // a race between the thread and the new color assignment.
  spdlog::debug("StatusLed: setMode color={} blink={}", static_cast<int>(color), blink);
  stopBlinking();

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

  if (blink) {
    startBlinking();
  }
}

void StatusLed::setOff() {
  stopBlinking();
  act.setOff();
  pwr.setOff();
}

void StatusLed::startBlinking() {
  blinkThread = std::jthread{[this](std::stop_token stop) {
    std::mutex sleepMutex;
    std::condition_variable_any sleepCv;
    bool on = true;
    spdlog::debug("blink thread started");
    while (!stop.stop_requested()) {
      on ? pwr.setOn() : pwr.setOff();
      std::unique_lock lock{sleepMutex};
      sleepCv.wait_for(lock, stop, blinkPeriod / 2, [] { return false; });
      on = !on;
    }
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

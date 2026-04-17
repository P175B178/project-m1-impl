#include "GpioBuzzer.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <ranges>
#include <thread>

#include <spdlog/spdlog.h>

using namespace std::chrono_literals;

namespace {
// Passive buzzer needs a square wave — tone frequency determines pitch.
constexpr int toneFreqHz = 2000;

constexpr auto tonePeriod = std::chrono::microseconds{1'000'000 / toneFreqHz};

// GPIO 17, physical pin 11 — fixed by hardware wiring.
constexpr unsigned int gpioPin = 17;

// Beep timing constants.
constexpr auto shortBeepDuration = 100ms;
constexpr auto shortBeepPause    = 100ms;
constexpr auto longBeepDuration  = 500ms;
} // namespace

namespace warden::hardware {

GpioBuzzer::GpioBuzzer(const std::string &chipPath)
      : pin{chipPath, gpioPin, "buzzer"} {}

GpioBuzzer::~GpioBuzzer() {
  stopBeeping();
  pin.setLow();
}

void GpioBuzzer::setOff() {
  spdlog::debug("GpioBuzzer: setOff");
  stopBeeping();
  pin.setLow();
}

void GpioBuzzer::shortBeep(unsigned int count) {
  spdlog::debug("GpioBuzzer: {} short beep(s)", count);
  startBeeping(shortBeepDuration, count);
}

void GpioBuzzer::longBeep(unsigned int count) {
  spdlog::debug("GpioBuzzer: {} long beep(s)", count);
  startBeeping(longBeepDuration, count);
}

void GpioBuzzer::startBeeping(std::chrono::milliseconds duration, unsigned int count) {
  stopBeeping();
  beepThread = std::jthread{[this, duration, count](std::stop_token stop) {
    std::mutex sleepMutex;
    std::condition_variable_any sleepCv;

    for (auto beepIdx : std::views::iota(0U, count)) {
      if (stop.stop_requested()) {
        break;
      }
      emitTone(duration, stop);
      if (beepIdx + 1 < count) {
        std::unique_lock lock{sleepMutex};
        sleepCv.wait_for(lock, stop, shortBeepPause, [] { return false; });
      }
    }
  }};
}

void GpioBuzzer::stopBeeping() {
  if (beepThread.joinable()) {
    beepThread.request_stop();
    beepThread.join();
  }
}

void GpioBuzzer::emitTone(std::chrono::milliseconds duration, std::stop_token stop) {
  const auto end = std::chrono::steady_clock::now() + duration;
  while (!stop.stop_requested() && std::chrono::steady_clock::now() < end) {
    pin.setHigh();
    std::this_thread::sleep_for(tonePeriod / 2);
    pin.setLow();
    std::this_thread::sleep_for(tonePeriod / 2);
  }
  pin.setLow();
}

} // namespace warden::hardware

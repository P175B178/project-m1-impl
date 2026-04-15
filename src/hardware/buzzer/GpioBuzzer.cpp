#include "GpioBuzzer.hpp"

#include <chrono>
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

void GpioBuzzer::emitTone(std::chrono::milliseconds duration) {
  const auto end = std::chrono::steady_clock::now() + duration;
  while (std::chrono::steady_clock::now() < end) {
    pin.setHigh();
    std::this_thread::sleep_for(tonePeriod / 2);
    pin.setLow();
    std::this_thread::sleep_for(tonePeriod / 2);
  }
}

void GpioBuzzer::shortBeep(int count) {
  spdlog::debug("GpioBuzzer: {} short beep(s)", count);
  for (auto beepIdx : std::views::iota(0, count)) {
    emitTone(shortBeepDuration);
    if (beepIdx + 1 < count) {
      std::this_thread::sleep_for(shortBeepPause);
    }
  }
}

void GpioBuzzer::longBeep(int count) {
  spdlog::debug("GpioBuzzer: {} long beep(s)", count);
  for (auto beepIdx : std::views::iota(0, count)) {
    emitTone(longBeepDuration);
    if (beepIdx + 1 < count) {
      std::this_thread::sleep_for(shortBeepPause);
    }
  }
}

} // namespace warden::hardware

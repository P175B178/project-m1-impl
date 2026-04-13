#include "GpioBuzzer.hpp"

#include <chrono>
#include <thread>

#include <spdlog/spdlog.h>

using namespace std::chrono_literals;

namespace {
// Passive buzzer needs a square wave — tone frequency determines pitch.
constexpr int toneFreqHz = 2000;
constexpr auto tonePeriod = std::chrono::microseconds{1'000'000 / toneFreqHz};
// GPIO 17, physical pin 11 — fixed by hardware wiring.
constexpr unsigned int kGpioPin = 17;
} // namespace

namespace warden::hardware {

GpioBuzzer::GpioBuzzer(const std::string &chipPath) : pin_{chipPath, kGpioPin, "buzzer"} {}

void GpioBuzzer::beep(int count, int durationMs, int pauseMs) {
  spdlog::debug("GpioBuzzer: beep count={} duration={}ms pause={}ms", count, durationMs, pauseMs);
  for (int i = 0; i < count; ++i) {
    const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds{durationMs};
    while (std::chrono::steady_clock::now() < end) {
      pin_.setHigh();
      std::this_thread::sleep_for(tonePeriod / 2);
      pin_.setLow();
      std::this_thread::sleep_for(tonePeriod / 2);
    }
    if (i < count - 1) {
      std::this_thread::sleep_for(std::chrono::milliseconds{pauseMs});
    }
  }
}

} // namespace warden::hardware

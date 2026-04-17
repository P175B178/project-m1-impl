#pragma once

#include "hardware/Buzzer.hpp"

#include <spdlog/spdlog.h>

namespace warden::sim {

/// No-op buzzer that logs beep requests to the console.
class StubBuzzer final : public Buzzer {
public:
  void shortBeep(unsigned int count) override { spdlog::info("[BUZZER] {} short beep(s)", count); }

  void longBeep(unsigned int count) override { spdlog::info("[BUZZER] {} long beep(s)", count); }

  void setOff() override { spdlog::info("[BUZZER] off"); }
};

} // namespace warden::sim

#pragma once

#include "warden/Buzzer.hpp"

#include <spdlog/spdlog.h>

namespace warden::sim {

/// No-op buzzer that logs beep requests to the console.
class StubBuzzer final : public Buzzer {
public:
    void beep(int count, int durationMs, int pauseMs) override {
        spdlog::info("[BUZZER] {} beep(s), {}ms on, {}ms pause", count, durationMs, pauseMs);
    }
};

} // namespace warden::sim

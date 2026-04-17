#pragma once

#include "hardware/Buzzer.hpp"

#include <spdlog/spdlog.h>

/// Simulated buzzer — logs beep requests to the console.
class StubBuzzer : public Buzzer {
public:
    void shortBeep(unsigned int count) override {
        spdlog::info("[BUZZER] {} short beep(s)", count);
    }

    void longBeep(unsigned int count) override {
        spdlog::info("[BUZZER] {} long beep(s)", count);
    }

    void setOff() override {
        spdlog::info("[BUZZER] off");
    }
};

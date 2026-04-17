#pragma once

#include "hardware/Led.hpp"

#include <spdlog/spdlog.h>

/// Simulated LED — logs state changes to the console.
class StubLed : public Led {
public:
    void setMode(LedColor color, bool blink) override {
        const char* name = "?";
        switch (color) {
        case LedColor::Green:  name = "green";  break;
        case LedColor::Orange: name = "orange"; break;
        case LedColor::Red:    name = "red";    break;
        }
        spdlog::info("[LED] {}{}", name, blink ? " (blinking)" : "");
    }

    void setOff() override {
        spdlog::info("[LED] off");
    }
};

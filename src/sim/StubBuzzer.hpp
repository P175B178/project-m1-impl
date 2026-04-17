#pragma once

#include "hardware/Buzzer.hpp"

#include <iostream>

/// Simulated buzzer — prints beep requests to stdout.
class StubBuzzer : public Buzzer {
public:
    void shortBeep(unsigned int count) override {
        std::cout << "[BUZZER] " << count << " short beep(s)" << std::endl;
    }

    void longBeep(unsigned int count) override {
        std::cout << "[BUZZER] " << count << " long beep(s)" << std::endl;
    }

    void setOff() override {
        std::cout << "[BUZZER] OFF" << std::endl;
    }
};

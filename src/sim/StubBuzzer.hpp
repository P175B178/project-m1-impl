#pragma once

#include "hardware/Buzzer.hpp"

#include <iostream>

/// Simulated buzzer — prints beep requests to stdout.
class StubBuzzer final : public Buzzer {
public:
  void shortBeep(unsigned int count) override { std::cout << "[BUZZER] " << count << " short beep(s)\n"; }

  void longBeep(unsigned int count) override { std::cout << "[BUZZER] " << count << " long beep(s)\n"; }

  void setOff() override { std::cout << "[BUZZER] OFF\n"; }
};

#pragma once

#include "GpioPin.hpp"
#include "warden/Buzzer.hpp"

#include <string>

namespace warden::hardware {

/// Passive buzzer driven by a single GPIO output pin.
/// Emits a tone by toggling the pin at a fixed frequency.
class GpioBuzzer : public warden::Buzzer {
public:
    /// @param chipPath  GPIO chip device path, e.g. "/dev/gpiochip4"
    explicit GpioBuzzer(const std::string& chipPath);

    void beep(int count, int durationMs, int pauseMs) override;

private:
    GpioPin pin_;
};

} // namespace warden::hardware

#pragma once

#include "hardware/Led.hpp"

#include <optional>

namespace warden::test {

/// Simple mock LED for unit tests. Records the last color and blink state set.
class MockLed : public Led {
public:
    void setMode(LedColor color, bool blink = false) override {
        lastColor = color;
        lastBlink = blink;
        off       = false;
    }

    void setOff() override {
        off       = true;
        lastColor = std::nullopt;
        lastBlink = false;
    }

    [[nodiscard]] std::optional<LedColor> getLastColor() const noexcept { return lastColor; }
    [[nodiscard]] bool getLastBlink() const noexcept { return lastBlink; }
    [[nodiscard]] bool isOff()        const noexcept { return off; }

private:
    std::optional<LedColor> lastColor;
    bool                    lastBlink{false};
    bool                    off{false};
};

} // namespace warden::test

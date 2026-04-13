#pragma once

#include "warden/Led.hpp"

#include <optional>

namespace warden::test {

/// Simple mock LED for unit tests. Records the last color and blink state set.
class MockLed : public Led {
public:
    void setMode(LedColor color, bool blink = false) override {
        lastColor_ = color;
        lastBlink_ = blink;
        isOff_     = false;
    }

    void setOff() override {
        isOff_     = true;
        lastColor_ = std::nullopt;
        lastBlink_ = false;
    }

    [[nodiscard]] std::optional<LedColor> lastColor() const noexcept { return lastColor_; }
    [[nodiscard]] bool lastBlink() const noexcept { return lastBlink_; }
    [[nodiscard]] bool isOff()     const noexcept { return isOff_; }

private:
    std::optional<LedColor> lastColor_;
    bool                    lastBlink_{false};
    bool                    isOff_{false};
};

} // namespace warden::test

#pragma once

#include "warden/Buzzer.hpp"

namespace warden::test {

/// Simple mock buzzer for unit tests. Records the arguments from the last beep() call.
class MockBuzzer : public Buzzer {
public:
    void beep(int count, int durationMs, int pauseMs) override {
        lastCount_      = count;
        lastDurationMs_ = durationMs;
        lastPauseMs_    = pauseMs;
        callCount_++;
    }

    [[nodiscard]] int lastCount()      const noexcept { return lastCount_; }
    [[nodiscard]] int lastDurationMs() const noexcept { return lastDurationMs_; }
    [[nodiscard]] int lastPauseMs()    const noexcept { return lastPauseMs_; }
    [[nodiscard]] int callCount()      const noexcept { return callCount_; }

private:
    int lastCount_{0};
    int lastDurationMs_{0};
    int lastPauseMs_{0};
    int callCount_{0};
};

} // namespace warden::test

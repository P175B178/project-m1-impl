#pragma once

#include "warden/Buzzer.hpp"

namespace warden::test {

/// Simple mock buzzer for unit tests. Records the arguments from the last beep() call.
class MockBuzzer : public Buzzer {
public:
    void beep(int count, int durationMs, int pauseMs) override {
        lastCount      = count;
        lastDurationMs = durationMs;
        lastPauseMs    = pauseMs;
        callCount++;
    }

    [[nodiscard]] int getLastCount()      const noexcept { return lastCount; }
    [[nodiscard]] int getLastDurationMs() const noexcept { return lastDurationMs; }
    [[nodiscard]] int getLastPauseMs()    const noexcept { return lastPauseMs; }
    [[nodiscard]] int getCallCount()      const noexcept { return callCount; }

private:
    int lastCount{0};
    int lastDurationMs{0};
    int lastPauseMs{0};
    int callCount{0};
};

} // namespace warden::test

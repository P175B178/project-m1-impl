#pragma once

#include "hardware/Sensor.hpp"

#include <optional>

namespace warden::test {

/// Simple mock sensor for unit tests. Preset the next return value with
/// setNextReading() or setNextError() before calling read().
class MockSensor : public Sensor {
public:
    void setNextReading(Reading r) {
        nextReading = r;
        nextError   = std::nullopt;
    }

    void setNextError(SensorError e) {
        nextError   = e;
        nextReading = std::nullopt;
    }

    [[nodiscard]] std::expected<Reading, SensorError> read() const override {
        if (nextError) { return std::unexpected{*nextError}; }
        return nextReading.value_or(Reading{.temperature = 20.0F, .humidity = 50.0F, .timestamp = {}});
    }

private:
    std::optional<Reading>     nextReading{Reading{.temperature = 20.0F, .humidity = 50.0F, .timestamp = {}}};
    std::optional<SensorError> nextError;
};

} // namespace warden::test

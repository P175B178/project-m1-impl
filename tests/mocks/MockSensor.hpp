#pragma once

#include "warden/Sensor.hpp"

#include <optional>

namespace warden::test {

/// Simple mock sensor for unit tests. Preset the next return value with
/// setNextReading() or setNextError() before calling read().
class MockSensor : public Sensor {
public:
    void setNextReading(Reading r) {
        nextReading_ = r;
        nextError_   = std::nullopt;
    }

    void setNextError(SensorError e) {
        nextError_   = e;
        nextReading_ = std::nullopt;
    }

    [[nodiscard]] std::expected<Reading, SensorError> read() const override {
        if (nextError_) return std::unexpected{*nextError_};
        return nextReading_.value_or(Reading{20.0f, 50.0f, {}});
    }

private:
    std::optional<Reading>     nextReading_{Reading{20.0f, 50.0f, {}}};
    std::optional<SensorError> nextError_;
};

} // namespace warden::test

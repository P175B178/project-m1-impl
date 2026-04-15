#pragma once

#include "hardware/Sensor.hpp"

#include <gmock/gmock.h>

namespace warden::test {

class MockSensor : public Sensor {
public:
    MOCK_METHOD((std::expected<Reading, SensorError>), read, (), (const, override));
};

} // namespace warden::test

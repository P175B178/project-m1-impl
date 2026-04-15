#pragma once

#include "hardware/Buzzer.hpp"

#include <gmock/gmock.h>

namespace warden::test {

class MockBuzzer : public Buzzer {
public:
    MOCK_METHOD(void, shortBeep, (int count), (override));
    MOCK_METHOD(void, longBeep,  (int count), (override));
};

} // namespace warden::test

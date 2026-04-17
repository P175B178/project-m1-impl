#pragma once

#include "hardware/Buzzer.hpp"

#include <gmock/gmock.h>

class MockBuzzer : public Buzzer {
public:
    MOCK_METHOD(void, shortBeep, (unsigned int count), (override));
    MOCK_METHOD(void, longBeep, (unsigned int count), (override));
    MOCK_METHOD(void, setOff, (), (override));
};

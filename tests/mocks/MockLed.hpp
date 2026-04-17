#pragma once

#include "hardware/Led.hpp"

#include <gmock/gmock.h>

class MockLed : public Led {
public:
    MOCK_METHOD(void, setMode, (LedColor color, bool blink), (override));
    MOCK_METHOD(void, setOff, (), (override));
};

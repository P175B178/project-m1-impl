#pragma once

#include "hardware/Led.hpp"

#include <gmock/gmock.h>

namespace warden::test {

class MockLed : public Led {
public:
    MOCK_METHOD(void, setMode, (LedColor color, bool blink), (override));
    MOCK_METHOD(void, setOff, (), (override));
};

} // namespace warden::test

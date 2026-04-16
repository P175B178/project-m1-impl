#pragma once

#include "LedColor.hpp"

/// Abstract LED interface.
class Led {
public:
    Led()                       = default;
    virtual ~Led()              = default;

    virtual void setMode(LedColor color, bool blink) = 0;
    virtual void setOff() = 0;
};

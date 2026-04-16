#pragma once

/// Abstract buzzer interface.
class Buzzer {
public:
    Buzzer()                          = default;
    virtual ~Buzzer()                 = default;

    virtual void shortBeep(unsigned int count) = 0;
    virtual void longBeep(unsigned int count) = 0;
    virtual void setOff() = 0;
};

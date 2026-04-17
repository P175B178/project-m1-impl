#pragma once

namespace warden {

/// Abstract buzzer interface.
class Buzzer {
public:
  Buzzer()                          = default;
  virtual ~Buzzer()                 = default;
  Buzzer(const Buzzer &)            = delete;
  Buzzer &operator=(const Buzzer &) = delete;
  Buzzer(Buzzer &&)                 = delete;
  Buzzer &operator=(Buzzer &&)      = delete;

  virtual void shortBeep(unsigned int count) = 0;
  virtual void longBeep(unsigned int count)  = 0;
  virtual void setOff()                      = 0;
};

} // namespace warden

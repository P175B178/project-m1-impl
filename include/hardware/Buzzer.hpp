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

  /// Emit one or more short beeps. Non-blocking — returns immediately.
  /// A new call cancels any beep currently in progress.
  /// @param count Number of beeps to emit.
  virtual void shortBeep(int count) = 0;

  /// Emit one or more long beeps. Non-blocking — returns immediately.
  /// A new call cancels any beep currently in progress.
  /// @param count Number of beeps to emit.
  virtual void longBeep(int count) = 0;
};

} // namespace warden

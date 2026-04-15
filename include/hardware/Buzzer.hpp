#pragma once

namespace warden {

/// Abstract buzzer interface.
///
/// All beep methods are non-blocking — they return immediately and drive
/// the sequence in the background. A new call always cancels the current
/// sequence, so the buzzer always reflects the most recent request.
class Buzzer {
public:
  Buzzer()                          = default;
  virtual ~Buzzer()                 = default;
  Buzzer(const Buzzer &)            = delete;
  Buzzer &operator=(const Buzzer &) = delete;
  Buzzer(Buzzer &&)                 = delete;
  Buzzer &operator=(Buzzer &&)      = delete;

  /// Emit one or more short beeps. Non-blocking — returns immediately.
  /// Cancels any beep currently in progress before starting the new sequence.
  /// @param count Number of beeps to emit.
  virtual void shortBeep(int count) = 0;

  /// Emit one or more long beeps. Non-blocking — returns immediately.
  /// Cancels any beep currently in progress before starting the new sequence.
  /// @param count Number of beeps to emit.
  virtual void longBeep(int count) = 0;

  /// Silence the buzzer immediately. Cancels any beep currently in progress.
  virtual void setOff() = 0;
};

} // namespace warden

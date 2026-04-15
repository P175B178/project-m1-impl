#pragma once

namespace warden {

/// Abstract buzzer interface.
class Buzzer {
public:
  virtual ~Buzzer() = default;

  /// Emit a series of beeps.
  /// @param count      Number of beeps.
  /// @param durationMs Duration of each beep in milliseconds.
  /// @param pauseMs    Pause between beeps in milliseconds.
  virtual void beep(int count, int durationMs, int pauseMs) = 0;
};

} // namespace warden

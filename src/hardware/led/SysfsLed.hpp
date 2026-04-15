#pragma once

#include <string>

namespace warden::hardware {

/// Controls a single Linux sysfs LED (/sys/class/leds/<name>/).
///
/// Takes ownership of the LED by setting its kernel trigger to "none",
/// which disables any kernel-managed behaviour (heartbeat, mmc activity).
/// Restores the LED to off on destruction.
///
/// The inverted flag handles a hardware quirk on the Pi 5 ACT LED where
/// brightness 0 = on and brightness 1 = off, opposite to the convention.
class SysfsLed {
public:
  /// @param name     LED name as it appears under /sys/class/leds/, e.g. "ACT"
  /// @param inverted true if 0=on, 1=off (e.g. Pi 5 ACT LED)
  explicit SysfsLed(std::string name, bool inverted = false);
  ~SysfsLed();

  SysfsLed(const SysfsLed &) = delete;
  SysfsLed &operator=(const SysfsLed &) = delete;
  SysfsLed(SysfsLed &&) = delete;
  SysfsLed &operator=(SysfsLed &&) = delete;

  void setOn() noexcept;
  void setOff() noexcept;

private:
  static void writeFile(const std::string &path, const char *value) noexcept;

  std::string basePath; // /sys/class/leds/<name>
  bool inverted;
};

} // namespace warden::hardware

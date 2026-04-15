#include "SysfsLed.hpp"

#include <fstream>
#include <spdlog/spdlog.h>
#include <string>

namespace warden::hardware {

void SysfsLed::writeFile(const std::string &path, const char *value) noexcept {
  std::ofstream f{path};
  if (!f) {
    spdlog::warn("SysfsLed: cannot write to {}", path);
    return;
  }
  f << value;
}

SysfsLed::SysfsLed(std::string name, bool inverted)
      : basePath{"/sys/class/leds/" + name}
      , inverted{inverted} {
  spdlog::debug("SysfsLed: init '{}' (inverted={})", name, inverted);
  writeFile(basePath + "/trigger", "none");
  setOff();
}

SysfsLed::~SysfsLed() { setOff(); }

void SysfsLed::setOn() noexcept { writeFile(basePath + "/brightness", inverted ? "0" : "1"); }

void SysfsLed::setOff() noexcept { writeFile(basePath + "/brightness", inverted ? "1" : "0"); }

} // namespace warden::hardware

#include "SysfsLed.hpp"

#include <fstream>
#include <spdlog/spdlog.h>
#include <string>

namespace warden::hardware {

void SysfsLed::writeFile(const std::string& path, const char* value) noexcept {
    std::ofstream f{path};
    if (!f) {
        spdlog::warn("SysfsLed: cannot write to {}", path);
        return;
    }
    f << value;
}

SysfsLed::SysfsLed(std::string name, bool inverted)
    : basePath_{"/sys/class/leds/" + name}
    , inverted_{inverted}
{
    spdlog::debug("SysfsLed: init '{}' (inverted={})", name, inverted);
    writeFile(basePath_ + "/trigger", "none");
    setOff();
}

SysfsLed::~SysfsLed() {
    setOff();
}

void SysfsLed::setOn() noexcept {
    writeFile(basePath_ + "/brightness", inverted_ ? "0" : "1");
}

void SysfsLed::setOff() noexcept {
    writeFile(basePath_ + "/brightness", inverted_ ? "1" : "0");
}

} // namespace warden::hardware

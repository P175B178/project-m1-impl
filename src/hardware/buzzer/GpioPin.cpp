#include "GpioPin.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace warden::hardware {

// Custom deleters for libgpiod C types — used with unique_ptr to guarantee
// cleanup even if an intermediate step throws.
struct LineConfigDeleter {
  void operator()(gpiod_line_config *ptr) const noexcept { gpiod_line_config_free(ptr); }
};
struct LineSettingsDeleter {
  void operator()(gpiod_line_settings *ptr) const noexcept { gpiod_line_settings_free(ptr); }
};
struct RequestConfigDeleter {
  void operator()(gpiod_request_config *ptr) const noexcept { gpiod_request_config_free(ptr); }
};

GpioPin::GpioPin(const std::string &chipPath, unsigned int lineOffset, const std::string &consumer)
      : offset{lineOffset} {
  chip = gpiod_chip_open(chipPath.c_str());
  if (!chip) {
    throw std::runtime_error{"Cannot open GPIO chip: " + chipPath};
  }

  auto lineSettings = std::unique_ptr<gpiod_line_settings, LineSettingsDeleter>{gpiod_line_settings_new()};
  if (!lineSettings) {
    throw std::runtime_error{"Cannot allocate line settings"};
  }
  gpiod_line_settings_set_direction(lineSettings.get(), GPIOD_LINE_DIRECTION_OUTPUT);
  gpiod_line_settings_set_output_value(lineSettings.get(), GPIOD_LINE_VALUE_INACTIVE);

  auto lineConfig = std::unique_ptr<gpiod_line_config, LineConfigDeleter>{gpiod_line_config_new()};
  if (!lineConfig) {
    throw std::runtime_error{"Cannot allocate line config"};
  }
  gpiod_line_config_add_line_settings(lineConfig.get(), &lineOffset, 1, lineSettings.get());

  auto reqConfig = std::unique_ptr<gpiod_request_config, RequestConfigDeleter>{gpiod_request_config_new()};
  if (!reqConfig) {
    throw std::runtime_error{"Cannot allocate request config"};
  }
  gpiod_request_config_set_consumer(reqConfig.get(), consumer.c_str());

  request = gpiod_chip_request_lines(chip, reqConfig.get(), lineConfig.get());
  if (!request) {
    throw std::runtime_error{"Cannot request GPIO line " + std::to_string(lineOffset)};
  }
}

GpioPin::~GpioPin() {
  if (request) {
    gpiod_line_request_release(request);
  }
  if (chip) {
    gpiod_chip_close(chip);
  }
}

void GpioPin::setHigh() { gpiod_line_request_set_value(request, offset, GPIOD_LINE_VALUE_ACTIVE); }

void GpioPin::setLow() { gpiod_line_request_set_value(request, offset, GPIOD_LINE_VALUE_INACTIVE); }

} // namespace warden::hardware

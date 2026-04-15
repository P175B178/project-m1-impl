#include "Dht22Sensor.hpp"

#include <chrono>
#include <fstream>
#include <string>

#include <spdlog/spdlog.h>

namespace warden::hardware {

Dht22Sensor::Dht22Sensor(
    std::string sysfsBasePath, float minTemperature, float maxTemperature, float minHumidity, float maxHumidity)
    : sysfsBasePath{std::move(sysfsBasePath)}, minTemperature{minTemperature}, maxTemperature{maxTemperature},
      minHumidity{minHumidity}, maxHumidity{maxHumidity} {}

std::expected<warden::Reading, warden::SensorError> Dht22Sensor::read() const {
  // Kernel IIO driver returns temperature in millidegrees C (scale: 0.001)
  auto temp = readSysfsValue(sysfsBasePath + "/in_temp_input", 0.001F);
  if (!temp) {
    return std::unexpected{temp.error()};
  }

  // Humidity in 0.001 % (scale: 0.001)
  auto hum = readSysfsValue(sysfsBasePath + "/in_humidityrelative_input", 0.001F);
  if (!hum) {
    return std::unexpected{hum.error()};
  }

  // Validate plausible physical ranges
  if (*temp < minTemperature || *temp > maxTemperature) {
    return std::unexpected{warden::SensorError::InvalidData};
  }
  if (*hum < minHumidity || *hum > maxHumidity) {
    return std::unexpected{warden::SensorError::InvalidData};
  }

  return warden::Reading{.temperature = *temp, .humidity = *hum, .timestamp = std::chrono::system_clock::now()};
}

std::expected<float, warden::SensorError> Dht22Sensor::readSysfsValue(const std::string &path, float scale) const {
  std::ifstream file{path};
  if (!file.is_open()) {
    return std::unexpected{warden::SensorError::ReadFailure};
  }

  int raw{};
  if (!(file >> raw)) {
    return std::unexpected{warden::SensorError::ReadFailure};
  }

  spdlog::debug("Dht22Sensor: raw={} scale={} value={:.3f} path={}", raw, scale, raw * scale, path);
  return static_cast<float>(raw) * scale;
}

} // namespace warden::hardware

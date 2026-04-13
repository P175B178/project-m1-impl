#include "Dht22Sensor.hpp"

#include <chrono>
#include <fstream>
#include <string>

#include <spdlog/spdlog.h>

namespace warden::hardware {

Dht22Sensor::Dht22Sensor(
    std::string sysfsBasePath, float minTemperature, float maxTemperature, float minHumidity, float maxHumidity)
    : sysfsBasePath_{std::move(sysfsBasePath)}, minTemperature_{minTemperature}, maxTemperature_{maxTemperature},
      minHumidity_{minHumidity}, maxHumidity_{maxHumidity} {}

std::expected<warden::Reading, warden::SensorError> Dht22Sensor::read() const {
  // Kernel IIO driver returns temperature in millidegrees C (scale: 0.001)
  auto temp = readSysfsValue(sysfsBasePath_ + "/in_temp_input", 0.001f);
  if (!temp) {
    return std::unexpected{temp.error()};
  }

  // Humidity in 0.001 % (scale: 0.001)
  auto hum = readSysfsValue(sysfsBasePath_ + "/in_humidityrelative_input", 0.001f);
  if (!hum) {
    return std::unexpected{hum.error()};
  }

  // Validate plausible physical ranges
  if (*temp < minTemperature_ || *temp > maxTemperature_) {
    return std::unexpected{warden::SensorError::InvalidData};
  }
  if (*hum < minHumidity_ || *hum > maxHumidity_) {
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

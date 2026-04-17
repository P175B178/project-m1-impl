#include "Dht22Sensor.hpp"

#include <chrono>
#include <fstream>
#include <string>

#include <spdlog/spdlog.h>

namespace {
// DHT22 physical limits — values outside these mean the sensor or driver is broken.
constexpr float minTemperature = -40.0F;
constexpr float maxTemperature = 80.0F;
constexpr float minHumidity    = 0.0F;
constexpr float maxHumidity    = 100.0F;
} // namespace

namespace warden::hardware {

Dht22Sensor::Dht22Sensor(std::string sysfsBasePath)
      : sysfsBasePath{std::move(sysfsBasePath)} {}

std::expected<warden::Reading, warden::SensorError> Dht22Sensor::read() const {
  // Kernel IIO driver returns temperature in millidegrees C (scale: 0.001)
  const auto tempResult = readSysfsValue(sysfsBasePath + "/in_temp_input", 0.001F);
  if (!tempResult) {
    return std::unexpected{tempResult.error()};
  }

  // Humidity in 0.001 % (scale: 0.001)
  const auto humResult = readSysfsValue(sysfsBasePath + "/in_humidityrelative_input", 0.001F);
  if (!humResult) {
    return std::unexpected{humResult.error()};
  }

  const float temperature = tempResult.value();
  const float humidity    = humResult.value();

  if (temperature < minTemperature || temperature > maxTemperature || humidity < minHumidity ||
      humidity > maxHumidity) {
    return std::unexpected{warden::SensorError::InvalidData};
  }

  return warden::Reading{
      .temperature = temperature, .humidity = humidity, .timestamp = std::chrono::system_clock::now()};
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

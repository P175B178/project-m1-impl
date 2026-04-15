#include "ConfigLoader.hpp"

#include <toml++/toml.hpp>

namespace warden {

std::expected<Config, std::string> ConfigLoader::load(std::string_view path) {
  toml::table table;
  try {
    table = toml::parse_file(path);
  } catch (const toml::parse_error &e) {
    return std::unexpected{std::string{e.description()}};
  }

  const Config defaults{};
  Config config{};

  config.readInterval    = std::chrono::seconds{table["read_interval_s"].value_or(defaults.readInterval.count())};
  config.averagingWindow = static_cast<std::size_t>(table["averaging_window"].value_or(defaults.averagingWindow));

  // thresholds
  config.temperatureThreshold = table["thresholds"]["temperature"].value_or(defaults.temperatureThreshold);
  config.humidityThreshold    = table["thresholds"]["humidity"].value_or(defaults.humidityThreshold);
  // validation
  config.minTemperature       = table["sensor_validation"]["min_temperature"].value_or(defaults.minTemperature);
  config.maxTemperature       = table["sensor_validation"]["max_temperature"].value_or(defaults.maxTemperature);
  config.minHumidity          = table["sensor_validation"]["min_humidity"].value_or(defaults.minHumidity);
  config.maxHumidity          = table["sensor_validation"]["max_humidity"].value_or(defaults.maxHumidity);

  return config;
}

} // namespace warden

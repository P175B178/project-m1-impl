#include "warden/ConfigLoader.hpp"

#include <toml++/toml.hpp>

namespace warden {

std::expected<Config, std::string> ConfigLoader::load(std::string_view path) {
    toml::table table;
    try {
        table = toml::parse_file(path);
    } catch (const toml::parse_error& e) {
        return std::unexpected{std::string{e.description()}};
    }

    Config config{};

    config.readInterval    = std::chrono::seconds{table["read_interval_s"].value_or(5)};
    config.averagingWindow = static_cast<std::size_t>(table["averaging_window"].value_or(10));

    config.temperatureThreshold = table["thresholds"]["temperature"].value_or(28.0f);
    config.humidityThreshold    = table["thresholds"]["humidity"].value_or(70.0f);

    config.minTemperature = table["sensor_validation"]["min_temperature"].value_or(-40.0f);
    config.maxTemperature = table["sensor_validation"]["max_temperature"].value_or(80.0f);
    config.minHumidity    = table["sensor_validation"]["min_humidity"].value_or(0.0f);
    config.maxHumidity    = table["sensor_validation"]["max_humidity"].value_or(100.0f);

    return config;
}

} // namespace warden

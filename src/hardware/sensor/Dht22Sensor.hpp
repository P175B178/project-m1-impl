#pragma once

#include "warden/Sensor.hpp"

#include <string>

namespace warden::hardware {

/// Reads temperature and humidity from a DHT22 via the Linux kernel IIO driver.
///
/// Setup (add to /boot/firmware/config.txt, then reboot):
///   dtoverlay=dht11,gpiopin=4
///
/// After loading, the kernel exposes readings via sysfs:
///   /sys/bus/iio/devices/iio:device0/in_temp_input          (millidegrees C)
///   /sys/bus/iio/devices/iio:device0/in_humidityrelative_input  (0.001 %)
///
/// The device path may vary — if multiple IIO devices are present, check
/// /sys/bus/iio/devices/ and pass the correct path to the constructor.
class Dht22Sensor : public warden::Sensor {
public:
    /// Validation bounds are constructor parameters rather than hardcoded
    /// so they can be driven from config — different deployments may have
    /// different plausible ranges (e.g. outdoor vs. server room).
    /// Defaults cover the full physical range of the DHT22.
    ///
    /// @param sysfsBasePath  e.g. "/sys/bus/iio/devices/iio:device0"
    /// @param minTemperature Validation lower bound in °C
    /// @param maxTemperature Validation upper bound in °C
    /// @param minHumidity    Validation lower bound in %
    /// @param maxHumidity    Validation upper bound in %
    explicit Dht22Sensor(std::string sysfsBasePath,
                         float minTemperature = -40.0f,
                         float maxTemperature =  80.0f,
                         float minHumidity    =   0.0f,
                         float maxHumidity    = 100.0f);

    [[nodiscard]] std::expected<warden::Reading, warden::SensorError> read() const override;

private:
    [[nodiscard]] std::expected<float, warden::SensorError> readSysfsValue(const std::string& path,
                                                                             float scale) const;

    std::string sysfsBasePath_;
    float minTemperature_;
    float maxTemperature_;
    float minHumidity_;
    float maxHumidity_;
};

} // namespace warden::hardware

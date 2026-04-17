#pragma once

#include "hardware/Sensor.hpp"

#include <string>

namespace warden::hardware {

/// Reads temperature and humidity from a DHT22 via the Linux kernel IIO driver.
///
/// Setup (add to /boot/firmware/config.txt, then reboot):
///   dtoverlay=dht11,gpiopin=4
///   (The dht11 overlay name is correct — it supports both DHT11 and DHT22.)
///
/// After loading, the kernel exposes readings via sysfs:
///   /sys/bus/iio/devices/iio:device0/in_temp_input          (millidegrees C)
///   /sys/bus/iio/devices/iio:device0/in_humidityrelative_input  (0.001 %)
class Dht22Sensor : public warden::Sensor {
public:
  explicit Dht22Sensor(std::string sysfsBasePath = "/sys/bus/iio/devices/iio:device0");

  [[nodiscard]] std::expected<warden::Reading, warden::SensorError> read() const override;

private:
  [[nodiscard]] std::expected<float, warden::SensorError> readSysfsValue(const std::string &path, float scale) const;

  std::string sysfsBasePath;
};

} // namespace warden::hardware

#pragma once

#include "Reading.hpp"

#include <expected>
#include <string_view>

enum class SensorError : std::uint8_t {
  ReadFailure, ///< Could not read from the hardware (I/O error, device absent)
  InvalidData  ///< Reading outside the sensor's physical limits (hardware/driver error)
};

[[nodiscard]] constexpr std::string_view sensorErrorToString(SensorError error) noexcept {
  switch (error) {
  case SensorError::ReadFailure:
    return "ReadFailure";
  case SensorError::InvalidData:
    return "InvalidData";
  }
  return "Unknown";
}

/// Abstract sensor interface.
///
/// Read errors are modelled as std::expected values rather than exceptions
/// because sensor failures are expected, recoverable conditions.
class Sensor {
public:
  Sensor()                          = default;
  virtual ~Sensor()                 = default;
  Sensor(const Sensor &)            = delete;
  Sensor &operator=(const Sensor &) = delete;
  Sensor(Sensor &&)                 = delete;
  Sensor &operator=(Sensor &&)      = delete;

  [[nodiscard]] virtual std::expected<Reading, SensorError> read() const = 0;
};

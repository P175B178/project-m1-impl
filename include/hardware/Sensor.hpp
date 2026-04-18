#pragma once

#include "Reading.hpp"

#include <expected>

enum class SensorError : std::uint8_t {
  ReadFailure, ///< Could not read from the hardware (I/O error, device absent)
  InvalidData  ///< Reading outside the sensor's physical limits (hardware/driver error)
};

/// Abstract sensor interface.
///
/// Read errors are modelled as std::expected values rather than exceptions
/// because sensor failures are expected, recoverable conditions.
class Sensor {
public:
  Sensor()          = default;
  virtual ~Sensor() = default;

  [[nodiscard]] virtual std::expected<Reading, SensorError> read() const = 0;
};

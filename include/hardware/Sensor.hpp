#pragma once

#include "Reading.hpp"

#include <expected>
#include <string_view>

namespace warden {

/// Hardware sensor errors are modelled as values rather than exceptions
/// because they are expected, recoverable conditions (transient read
/// failures, out-of-range readings) rather than programming errors.
enum class SensorError : std::uint8_t {
  ReadFailure, ///< Could not read from the hardware (I/O error, device absent)
  InvalidData  ///< Reading outside configured plausible physical bounds
};

[[nodiscard]] constexpr std::string_view sensorErrorToString(SensorError error) {
  switch (error) {
  case SensorError::ReadFailure: return "ReadFailure";
  case SensorError::InvalidData: return "InvalidData";
  }
  return "Unknown";
}

/// Abstract sensor interface.
///
/// Read errors are modelled as std::expected values rather than exceptions
/// because sensor failures are expected, recoverable conditions — not programming errors.
class Sensor {
public:
  Sensor()                          = default;
  virtual ~Sensor()                 = default;
  Sensor(const Sensor &)            = delete;
  Sensor &operator=(const Sensor &) = delete;
  Sensor(Sensor &&)                 = delete;
  Sensor &operator=(Sensor &&)      = delete;

  /// Read the current temperature and humidity from the sensor.
  /// @return A Reading on success, or a SensorError describing the failure.
  [[nodiscard]] virtual std::expected<Reading, SensorError> read() const = 0;
};

} // namespace warden

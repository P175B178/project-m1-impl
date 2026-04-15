#pragma once

#include "Reading.hpp"

#include <expected>
#include <string>

namespace warden {

/// Hardware sensor errors are modelled as values rather than exceptions
/// because they are expected, recoverable conditions (transient read
/// failures, out-of-range readings) rather than programming errors.
enum class SensorError : std::uint8_t {
  ReadFailure, ///< Could not read from the hardware (I/O error, device absent)
  InvalidData  ///< Reading outside configured plausible physical bounds
};

/// Abstract sensor interface.
class Sensor {
public:
  Sensor() = default;
  virtual ~Sensor() = default;
  Sensor(const Sensor &) = delete;
  Sensor &operator=(const Sensor &) = delete;
  Sensor(Sensor &&) = delete;
  Sensor &operator=(Sensor &&) = delete;

  [[nodiscard]] virtual std::expected<Reading, SensorError> read() const = 0;
};

} // namespace warden

#pragma once

#include <chrono>

namespace warden {

/// A single sensor sample.
struct Reading {
  float temperature{}; ///< Temperature in °C
  float humidity{};    ///< Relative humidity in %
  std::chrono::system_clock::time_point timestamp{};
};

} // namespace warden

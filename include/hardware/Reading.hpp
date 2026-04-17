#pragma once

#include <chrono>

/// A single sensor sample.
struct Reading {
  float temperature{}; ///< Temperature in degrees C
  float humidity{};    ///< Relative humidity in %
  std::chrono::system_clock::time_point timestamp;
};

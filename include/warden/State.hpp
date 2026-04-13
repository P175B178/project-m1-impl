#pragma once

#include <string_view>

namespace warden {

enum class State {
  Normal,  ///< No threshold exceeded
  Warning, ///< Exactly one threshold exceeded
  Alert    ///< Both thresholds exceeded
};

constexpr std::string_view stateToString(State s) noexcept {
  switch (s) {
  case State::Normal:
    return "Normal";
  case State::Warning:
    return "Warning";
  case State::Alert:
    return "Alert";
  }
  return "Unknown";
}

} // namespace warden

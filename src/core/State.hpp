#pragma once

#include <cstdint>
#include <string_view>

enum class State : std::uint8_t { Normal, Warning, Alert };

[[nodiscard]] constexpr std::string_view stateToString(State state) noexcept {
  switch (state) {
  case State::Normal:
    return "Normal";
  case State::Warning:
    return "Warning";
  case State::Alert:
    return "Alert";
  }
  return "Unknown";
}

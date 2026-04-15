#pragma once

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

namespace warden {

/// Circular buffer that keeps the last `capacity` values and computes
/// their average. The window size is set at runtime from the config file.
template <typename T> class AveragingBuffer {
public:
  explicit AveragingBuffer(std::size_t capacity) : buffer(capacity), capacity{capacity} {
    if (capacity == 0) {
      throw std::invalid_argument("Averaging window must be > 0");
    }
  }

  /// Add a new value, evicting the oldest if the buffer is full.
  void push(T value) noexcept {
    buffer[head] = value;
    head = (head + 1) % capacity;
    if (count < capacity) {
      ++count;
    }
  }

  /// Compute the mean of all stored values.
  /// Returns std::nullopt if the buffer is empty.
  [[nodiscard]] std::optional<T> average() const noexcept {
    if (count == 0) {
      return std::nullopt;
    }
    T sum{};
    for (std::size_t i = 0; i < count; ++i) {
      sum += buffer[i];
    }
    return sum / static_cast<T>(count);
  }

  [[nodiscard]] std::size_t size() const noexcept { return count; }
  [[nodiscard]] bool empty() const noexcept { return count == 0; }
  [[nodiscard]] bool full() const noexcept { return count == capacity; }

private:
  std::vector<T> buffer;
  std::size_t capacity;
  std::size_t head{0};
  std::size_t count{0};
};

} // namespace warden

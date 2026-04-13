#pragma once

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

namespace warden {

/// Circular buffer that keeps the last `capacity` values and computes
/// their average. The window size is set at runtime from the config file.
template <typename T>
class AveragingBuffer {
public:
    explicit AveragingBuffer(std::size_t capacity)
        : buffer_(capacity), capacity_{capacity} {
        if (capacity == 0) throw std::invalid_argument("Averaging window must be > 0");
    }

    /// Add a new value, evicting the oldest if the buffer is full.
    void push(T value) noexcept {
        buffer_[head_] = value;
        head_ = (head_ + 1) % capacity_;
        if (count_ < capacity_) ++count_;
    }

    /// Compute the mean of all stored values.
    /// Returns std::nullopt if the buffer is empty.
    [[nodiscard]] std::optional<T> average() const noexcept {
        if (count_ == 0) return std::nullopt;
        T sum{};
        for (std::size_t i = 0; i < count_; ++i) {
            sum += buffer_[i];
        }
        return sum / static_cast<T>(count_);
    }

    [[nodiscard]] std::size_t size()     const noexcept { return count_; }
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
    [[nodiscard]] bool        empty()    const noexcept { return count_ == 0; }
    [[nodiscard]] bool        full()     const noexcept { return count_ == capacity_; }

private:
    std::vector<T> buffer_;
    std::size_t    capacity_;
    std::size_t    head_{0};
    std::size_t    count_{0};
};

} // namespace warden

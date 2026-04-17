#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace warden {

/// Thread-safe blocking queue for a single producer / single consumer.
///
/// push() enqueues an item and wakes the consumer.
/// pop() blocks until an item is available or stop() is called.
/// Once stop() is called, pop() returns std::nullopt immediately,
/// discarding any remaining items.
template <typename T> class BlockingQueue {
public:
  void push(T item) {
    {
      std::scoped_lock lock{mutex};
      queue.push(std::move(item));
    }
    cv.notify_one();
  }

  /// Block until an item is available or stop() is called.
  /// Returns the item, or std::nullopt if stop() was called.
  [[nodiscard]] std::optional<T> pop() {
    std::unique_lock lock{mutex};
    cv.wait(lock, [this] { return !queue.empty() || stopped; });

    if (stopped) {
      return std::nullopt;
    }

    T item = std::move(queue.front());
    queue.pop();
    return item;
  }

  /// Unblock any waiting pop() call and prevent further blocking.
  /// Any items still in the queue are discarded — pop() returns std::nullopt immediately.
  void stop() {
    {
      std::scoped_lock lock{mutex};
      stopped = true;
    }
    cv.notify_all();
  }

private:
  std::queue<T> queue;
  std::mutex mutex;
  std::condition_variable cv;
  bool stopped{false};
};

} // namespace warden

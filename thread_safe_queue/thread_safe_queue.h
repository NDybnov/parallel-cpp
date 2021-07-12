#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

#include <optional>
#include <queue>


template <typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() {
  }

  void Push(const T& value) {
    queue_mutex_.lock();
    queue_.push(value);
    queue_mutex_.unlock();
  }

  T Pop() {
    queue_mutex_.lock();
    while (queue_.empty()) {
        queue_mutex_.unlock();
        std::this_thread::yield();
        queue_mutex_.lock();
    }
    auto value = queue_.front();
    queue_.pop();
    queue_mutex_.unlock();
    return value;
  }

  std::optional<T> TryPop() {
    queue_mutex_.lock();
    if (queue_.empty()) {
        queue_mutex_.unlock();
      return std::nullopt;
    }
    auto value = queue_.front();
    queue_.pop();
    queue_mutex_.unlock();
    return value;
  }


 private:
  std::mutex queue_mutex_;
  std::queue<T> queue_;
};


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
    std::unique_lock<std::mutex> queue_locker(queue_mutex_);
    queue_.push(value);
  }

  T Pop() {
    std::unique_lock<std::mutex> queue_locker(queue_mutex_);
    std::condition_variable cv;
    cv.wait(queue_locker, [&]{return !queue_.epmty();});
    auto value = queue_.front();
    queue_.pop();
    return value;
  }

  std::optional<T> TryPop() {
    std::unique_lock<std::mutex> queue_locker(queue_mutex_);
    if (queue_.empty()) {
      return std::nullopt;
    }
    auto value = queue_.front();
    queue_.pop();
    return value;
  }


 private:
  std::mutex queue_mutex_;
  std::queue<T> queue_;
};

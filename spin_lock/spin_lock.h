#pragma once

#include <atomic>
#include <thread>

class SpinLock {
 public:
  SpinLock() {}

  void Lock() {
    while (is_locked_.load() || is_locked_.exchange(true)) {
      std::this_thread::yield();
    }
  }

  void Unlock() {
    is_locked_.store(false);
  }

 private:
   std::atomic_bool is_locked_{false};
};

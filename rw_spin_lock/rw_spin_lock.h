#pragma once

#include <atomic>
#include <thread>


class RWSpinLock {
 public:
  RWSpinLock() {
  }

  void LockRead() {
    for (auto expected = counter_and_flag_.load(), auto desired = expected + 2;;) {
      while (expected & 1) {
        std::this_thread::yield();
        expected = counter_and_flag_.load();
        desired = expected + 2;
      }
      if (counter_and_flag_.compare_exchange_weak(expected, desired)) {
        break;
      }
    }
  }

  void UnlockRead() {
      counter_and_flag_.fetch_sub(2);
  }

  void LockWrite() {
    for (;;) {
        counter_and_flag_.fetch_or(1);
        while (counter_and_flag_.load() != 1) {
            std::this_thread::yield();
            counter_and_flag_.fetch_or(1);
        }
        unsigned int CAS_expected = 1;  // чтобы менялось
        if (counter_and_flag_.compare_exchange_weak(CAS_expected, 3)) {
            break;
        } else {
            std::this_thread::yield();
        }
    }
  }

  void UnlockWrite() {
    counter_and_flag_.store(0);
  }

 private:
    std::atomic<unsigned int> counter_and_flag_ = 0;
};

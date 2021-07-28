#pragma once

#include <vector>
#include <cstdint>
#include <atomic>
#include <thread>
#include <optional>


template <class T>
class MPMCQueue {
 public:
  explicit MPMCQueue(uint64_t max_size)
    : items_(max_size)
      , capacity_(max_size)
      {
        for(uint64_t i = 0; i < max_size ; ++i )
        {
          items_[i].version_ = i;
        }
      }

  bool Push(const T& value) {
    for (;;) {
        uint64_t head = head_.load();
        uint64_t idx = head & (capacity_ - 1);
        if (head == tail_.load() + capacity_) {
          return false;
        }
        if (items_[idx].version_ != head) {
            std::this_thread::yield();
            continue;
        } else {
            if (head_.compare_exchange_weak(head, head + 1)) {
              items_[idx].value_ = value;
              ++items_[idx].version_;
              return true;
            }
        }
    }
  }

  std::optional<T> Pop() {
    for (;;) {
        uint64_t tail = tail_.load();
        uint64_t idx = tail & (capacity_ - 1);
        if (tail == head_.load()) {
          return std::nullopt;
        }
        if (items_[idx].version_ != (tail + 1)) {
            std::this_thread::yield();
            continue;
        } else {
            if (tail_.compare_exchange_weak(tail, tail + 1)) {
              T value = items_[idx].value_;
              items_[idx].version_ += capacity_ - 1;
              return value;
            }
        }
    }
  }

 private:
    class Item
    {
      public:
        std::atomic<uint64_t> version_;
        T value_;
    };

    std::vector<Item> items_;
    uint64_t capacity_;

    std::atomic<uint64_t> head_{0};
    std::atomic<uint64_t> tail_{0};
};


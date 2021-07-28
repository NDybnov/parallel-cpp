#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <vector>


template <typename T>
class ThreadSafeVector {
 public:
  ThreadSafeVector() {
      realloc_.store(false);
  }

  T operator[](size_t index) const {
    vector_reader_mutex_.lock_shared();
    while (realloc_.load()) {
        vector_reader_mutex_.unlock_shared();
        std::this_thread::yield();
        vector_reader_mutex_.lock_shared();
    }
    T ans = vector_[index];
    vector_reader_mutex_.unlock_shared();
    return ans;
  }

  size_t Size() const {
    return vector_.size();
  }

  void PushBack(const T& value) {
    vector_writer_mutex_.lock();
    if (vector_.capacity() == vector_.size()) {
        realloc_.store(true);
        vector_reader_mutex_.lock();
        vector_.push_back(value);
        vector_reader_mutex_.unlock();
        realloc_.store(false);
    } else {
        vector_.push_back(value);
    }
    vector_writer_mutex_.unlock();
  }

 private:
  mutable std::shared_mutex vector_reader_mutex_;
  mutable std::mutex vector_writer_mutex_;
  mutable std::atomic_bool realloc_;
  std::vector<T> vector_;
};


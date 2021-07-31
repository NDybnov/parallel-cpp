#pragma once


#include <chrono>
#include <thread>
#include <mutex>

class Deadlock {
 public:
    Deadlock() {}

  void ThreadOne() {
      std::unique_lock<std::mutex> locker2(mutex2_);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      std::unique_lock<std::mutex> locker1(mutex1_);
  }

  void ThreadTwo() {
      std::unique_lock<std::mutex> locker1(mutex1_);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      std::unique_lock<std::mutex> locker2(mutex2_);
  }

 private:
  std::mutex mutex1_;
  std::mutex mutex2_;
};

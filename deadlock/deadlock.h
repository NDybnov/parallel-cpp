#pragma once


#include <chrono>
#include <thread>
#include <mutex>

class Deadlock {
 public:
    Deadlock() {}

  void ThreadOne() {
      m2_.lock();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      m1_.lock();
      m2_.unlock();
      m1_.unlock();
  }

  void ThreadTwo() {
      m1_.lock();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      m2_.lock();
      m2_.unlock();
      m1_.unlock();
  }

 private:
  std::mutex m1_;
  std::mutex m2_;
};

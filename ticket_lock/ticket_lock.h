#pragma once

#include <atomic>
#include <thread>

class TicketLock {
 public:
  TicketLock() {}

  void Lock() {
    std::atomic<int> cur = next_ticket_.fetch_add(1);
      while (now_serving_ != cur) {
        std::this_thread::yield();   
      }
  }

  void Unlock() {
    ++now_serving_;  
  }

 private:
  std::atomic<uint64_t> now_serving_, next_ticket_;
};

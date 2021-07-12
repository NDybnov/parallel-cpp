#pragma once

#include <ostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string_view>
#include <iostream>

class HelloWorld {
 public:
  static constexpr std::string_view kHelloPrefix = "Hello, World! From thread ";

  HelloWorld(size_t n_threads)
    :n_threads_(n_threads)
  {}

  void SayHello(std::ostream& os) {
    std::mutex os_mutex;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < n_threads_; ++i) {
        threads.emplace_back([&] () {
            os_mutex.lock();
            os << kHelloPrefix <<  std::this_thread::get_id() << '\n';
            os_mutex.unlock();
        });
    }
    for (size_t i = 0; i < n_threads_; ++i) {
        threads[i].join();
    }
  }

 private:
  const size_t n_threads_;
};

#pragma once

#include <condition_variable>
#include <exception>
#include <mutex>
#include <optional>

template <typename T>
class UnbufferedChannel{
 public:
  UnbufferedChannel() {}

  void Send(const T& value) {
      std::unique_lock<std::mutex> s_lock(sblock__);
      std::unique_lock<std::mutex> locker(ublock_);
      if (closed_) {
        throw std::runtime_error("Channel is closed");
      }
      placed_ = true;
      value_pointer_ = &value;
      ready_to_ship_.notify_one();
      grab_.wait(locker, [&] {
        return !placed_ || closed_;
      });
      if (placed_) {
        throw std::runtime_error("Channel is closed");
      }
  }

  std::optional<T> Recv() {
      std::unique_lock<std::mutex> r_lock(rblock_);
      std::unique_lock<std::mutex> locker(ublock_);
      ready_to_ship_.wait(locker, [&] {
        return placed_ || closed_;
      });
      if (closed_) {
        return std::nullopt;
      }
      placed_ = false;
      std::optional<T> ret_opt(*value_pointer_);
      grab_.notify_one();
      return ret_opt;
  }

  void Close() {
    std::unique_lock<std::mutex> locker(ublock_);
    closed_ = true;
    ready_to_ship_.notify_all();
    grab_.notify_all();
  }

 private:
  T const *value_pointer_;
  bool closed_ = false;
  bool placed_ = false;
  std::mutex ublock_;
  std::mutex sblock__;
  std::mutex rblock_;
  std::condition_variable ready_to_ship_;
  std::condition_variable grab_;

};

#pragma once

#include <mutex>


class Fork {
 public:
  Fork(size_t id) : id_(id) {
  }

  size_t Id() const {
    return id_;
  }

  void Get() {
    mutex_.lock();
  }

  bool TryGet() {
    return mutex_.try_lock();
  }

  void Put() {
    mutex_.unlock();
  }

 private:
  size_t id_;
  std::mutex mutex_;
};

class Philosopher {
 public:
  Philosopher(size_t id, Fork* left_fork, Fork* right_fork)
      : id_(id)
      , fork_left_(left_fork)
      , fork_right_(right_fork)
  {}

  size_t Id() const {
    return id_;
  }

  void Eat() {
    if (fork_left_->Id() > fork_right_->Id()) {
        std::swap(fork_left_, fork_right_);
    }
    fork_left_->Get();
    fork_right_->Get();
  }

  void Think() {
    fork_left_->Put();
    fork_right_->Put();
  }

 private:
  const size_t id_;
  Fork* fork_left_;
  Fork* fork_right_;
};

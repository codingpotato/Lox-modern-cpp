#ifndef LOX_STACK_H
#define LOX_STACK_H

#include <vector>

#include "contract.h"

namespace lox {

template <typename T, size_t Max_size>
struct stack {
  stack() noexcept : storage_(Max_size), count_{0} {}

  bool empty() const noexcept { return count_ == 0; }

  template <typename... Args>
  void push(Args&&... args) {
    if (count_ == Max_size) {
      throw runtime_error{"Stack overflow."};
    }
    storage_[count_++] = T{std::forward<Args>(args)...};
  }

  T& operator[](size_t pos) noexcept {
    ENSURES(pos < count_);
    return storage_[pos];
  }

  const T& pop() noexcept {
    ENSURES(count_ > 0);
    return storage_[--count_];
  }

  T& peek() noexcept { return storage_[count_ - 1]; }

 private:
  std::vector<T> storage_;
  size_t count_;
};

}  // namespace lox

#endif

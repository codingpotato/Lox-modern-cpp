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
  bool push(Args&&... args) noexcept {
    if (count_ < Max_size) {
      storage_[count_++] = T{std::forward<Args>(args)...};
      return true;
    }
    return false;
  }

  T& operator[](size_t pos) noexcept {
    ENSURES(pos < count_);
    return storage_[pos];
  }

  const T& pop() noexcept {
    ENSURES(count_ > 0);
    return storage_[--count_];
  }

  const T& peek() const noexcept { return storage_[count_ - 1]; }

 private:
  std::vector<T> storage_;
  size_t count_;
};

}  // namespace lox

#endif

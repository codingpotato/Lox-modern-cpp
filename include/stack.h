#ifndef LOX_STACK_H
#define LOX_STACK_H

#include <vector>

#include "contract.h"

namespace lox {

template <typename T, size_t Max_size>
struct stack {
  stack() noexcept : storage_(Max_size), size_{0} {}

  bool empty() const noexcept { return size_ == 0; }

  size_t size() const noexcept { return size_; }

  void resize(size_t size) noexcept {
    ENSURES(size < Max_size);
    size_ = size;
  }

  template <typename... Args>
  void push(Args&&... args) noexcept {
    ENSURES(size_ < Max_size);
    storage_[size_++] = T{std::forward<Args>(args)...};
  }

  T& operator[](size_t pos) noexcept {
    ENSURES(pos < size_);
    return storage_[pos];
  }

  const T& pop() noexcept {
    ENSURES(size_ > 0);
    return storage_[--size_];
  }

  T& peek(size_t distance = 0) noexcept {
    ENSURES(distance < size_);
    return storage_[size_ - distance - 1];
  }

 private:
  std::vector<T> storage_;
  size_t size_;
};

}  // namespace lox

#endif

#ifndef LOX_STACK_H
#define LOX_STACK_H

#include <utility>
#include <vector>

#include "contract.h"

namespace lox {

template <typename T, size_t Max_size>
class Stack {
 public:
  Stack() noexcept : storage(Max_size) {}

  bool empty() const noexcept { return count == 0; }
  size_t size() const noexcept { return count; }

  void resize(size_t size) noexcept {
    ENSURES(size <= Max_size);
    count = size;
  }

  template <typename... Args>
  void push(Args&&... args) noexcept {
    ENSURES(count <= Max_size);
    storage[count++] = T{std::forward<Args>(args)...};
  }

  const T& operator[](size_t pos) const noexcept {
    ENSURES(pos < count);
    return storage[pos];
  }

  T& operator[](size_t pos) noexcept {
    return const_cast<T&>(std::as_const(*this)[pos]);
  }

  T pop() noexcept {
    ENSURES(count > 0);
    return storage[--count];
  }

  const T& peek(size_t distance = 0) const noexcept {
    ENSURES(distance < count);
    return storage[count - distance - 1];
  }

  T& peek(size_t distance = 0) noexcept {
    return const_cast<T&>(std::as_const(*this).peek(distance));
  }

 private:
  std::vector<T> storage;
  size_t count = 0;
};

}  // namespace lox

#endif

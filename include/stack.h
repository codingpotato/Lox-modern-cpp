#ifndef LOX_STACK_H
#define LOX_STACK_H

#include <vector>

#include "contract.h"

namespace lox {

template <typename T, size_t Max_size>
struct Stack {
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

  T& operator[](size_t pos) noexcept {
    ENSURES(pos < count);
    return storage[pos];
  }

  const T& operator[](size_t pos) const noexcept {
    ENSURES(pos < count);
    return storage[pos];
  }

  const T& pop() noexcept {
    ENSURES(count > 0);
    return storage[--count];
  }

  T& peek(size_t distance = 0) noexcept {
    ENSURES(distance < count);
    return storage[count - distance - 1];
  }

  const T& back() const noexcept {
    ENSURES(count > 0);
    return storage[count - 1];
  }

 private:
  std::vector<T> storage;
  size_t count = 0;
};

}  // namespace lox

#endif

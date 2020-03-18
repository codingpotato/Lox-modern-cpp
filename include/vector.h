#ifndef LOX_VECTOR_H
#define LOX_VECTOR_H

#include <cstdlib>
#include <new>
#include <utility>

namespace lox {

template <typename T>
struct vector {
  vector() noexcept : capacity_{0}, count_{0}, values_{nullptr} {}

  ~vector() noexcept {
    if (values_) {
      for (auto index = 0; index < count_; ++index) {
        reinterpret_cast<T*>(&values_[index])->~T();
      }
      std::free(values_);
    }
  }

  int size() const noexcept { return count_; }

  const T& operator[](int index) const noexcept {
    return *reinterpret_cast<T*>(&values_[index]);
  }

  template <typename... Args>
  void emplace_back(Args&&... args) noexcept {
    if (count_ >= capacity_) {
      grow();
    }
    new (&values_[count_++]) T{std::forward<Args>(args)...};
  }

 private:
  void grow() noexcept {
    constexpr int initial_capacity = 8;
    capacity_ = capacity_ < initial_capacity ? initial_capacity : capacity_ * 2;
    values_ = reinterpret_cast<storage*>(
        std::realloc(values_, sizeof(storage) * capacity_));
  }

  using storage = std::aligned_storage_t<sizeof(T), alignof(T)>;

  int capacity_;
  int count_;
  storage* values_;
};

}  // namespace lox

#endif

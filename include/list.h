#ifndef LOX_LIST_H
#define LOX_LIST_H

#include <utility>

namespace lox {

template <typename T>
struct list {
  struct iterator {
    iterator(T* current) noexcept : current_{current} {}

    T* operator->() noexcept { return current_; }

    iterator& operator++() noexcept {
      current_ = current_->next();
      return *this;
    }

    friend bool operator!=(const iterator& lhs, const iterator& rhs) noexcept {
      return lhs.current_ != rhs.current_;
    }

   private:
    T* current_;
  };

  list() noexcept : head_{nullptr} {}

  ~list() noexcept {
    while (head_) {
      auto current = head_;
      head_ = head_->next();
      delete current;
    }
  }

  iterator begin() const noexcept { return head_; }
  iterator end() const noexcept { return nullptr; }

  template <typename... Args>
  void emplace(Args&&... args) noexcept {
    auto t = new T{std::forward<Args>(args)...};
    t->set_next(head_);
    head_ = t;
  }

 private:
  T* head_;
};

}  // namespace lox

#endif

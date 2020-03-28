#ifndef LOX_LIST_H
#define LOX_LIST_H

#include <utility>

#include "object.h"

namespace lox {

struct list {
  struct iterator {
    iterator(object* current) noexcept : current_{current} {}

    object* operator->() noexcept { return current_; }

    iterator& operator++() noexcept {
      current_ = current_->next;
      return *this;
    }

    friend bool operator!=(const iterator& lhs, const iterator& rhs) noexcept {
      return lhs.current_ != rhs.current_;
    }

   private:
    object* current_;
  };

  list() noexcept : head_{nullptr} {}

  ~list() noexcept {
    while (head_) {
      auto current = head_;
      head_ = head_->next;
      delete current;
    }
  }

  iterator begin() const noexcept { return head_; }
  iterator end() const noexcept { return nullptr; }

  void insert(object* obj) noexcept {
    obj->next = head_;
    head_ = obj;
  }

 private:
  object* head_;
};

}  // namespace lox

#endif

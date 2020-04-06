#ifndef LOX_LIST_H
#define LOX_LIST_H

#include <utility>

#include "object.h"

namespace lox {

struct List {
  struct Iterator {
    Iterator(Object* current) noexcept : current_{current} {}

    Object* operator->() noexcept { return current_; }

    Iterator& operator++() noexcept {
      current_ = current_->next;
      return *this;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) noexcept {
      return lhs.current_ != rhs.current_;
    }

   private:
    Object* current_;
  };

  List() noexcept : head_{nullptr} {}

  ~List() noexcept {
    while (head_) {
      auto current = head_;
      head_ = head_->next;
      delete current;
    }
  }

  Iterator begin() const noexcept { return head_; }
  Iterator end() const noexcept { return nullptr; }

  void insert(Object* obj) noexcept {
    obj->next = head_;
    head_ = obj;
  }

 private:
  Object* head_;
};

}  // namespace lox

#endif

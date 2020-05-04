#ifndef LOX_LIST_H
#define LOX_LIST_H

#include <iterator>

#include "contract.h"

namespace lox {

template <typename Node, bool Owened = true>
class List {
 public:
  class Iterator : public std::iterator<std::forward_iterator_tag, Node> {
   public:
    Iterator(Node* current) noexcept : current{current} {}

    operator Node*() noexcept { return current; }
    Node* operator->() noexcept { return current; }

    Iterator& operator++() noexcept {
      ENSURES(current);
      current = current->next;
      return *this;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) noexcept {
      return lhs.current != rhs.current;
    }

   private:
    Node* current;
  };

  ~List() noexcept {
    if constexpr (Owened) {
      while (head) {
        auto next = head->next;
        delete head;
        head = next;
      }
    }
  }

  bool empty() const noexcept { return head == nullptr; }

  Iterator begin() const noexcept { return head; }
  Iterator end() const noexcept { return nullptr; }

  void insert(Node* node, Node* after = nullptr) noexcept {
    ENSURES(node);
    if (after) {
      node->next = after->next;
      after->next = node;
    } else {
      node->next = head;
      head = node;
    }
  }

  template <typename Pred>
  void erase_if(Pred pred) noexcept {
    Node* previous = nullptr;
    Node* node = head;
    while (node != nullptr) {
      if (pred(node)) {
        auto erased = node;
        node = node->next;
        if (previous != nullptr) {
          previous->next = node;
        } else {
          head = node;
        }
        delete erased;
      } else {
        previous = node;
        node = node->next;
      }
    }
  }

 private:
  Node* head = nullptr;
};

}  // namespace lox

#endif

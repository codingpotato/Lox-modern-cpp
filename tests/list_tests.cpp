#include <doctest/doctest.h>

#include "list.h"

struct node {
  node(int value) noexcept : value_{value} {}

  int value() noexcept { return value_; }
  node* next() const noexcept { return next_; }
  void set_next(node* next) noexcept { next_ = next; }

 private:
  int value_;
  node* next_ = nullptr;
};

TEST_CASE("list") {
  lox::list<node> list;
  constexpr int count = 10;
  for (int i = 0; i < count; ++i) {
    list.emplace(i);
  }
  int expected = count - 1;
  for (auto it = list.begin(); it != list.end(); ++it) {
    CHECK_EQ(it->value(), expected--);
  }
}

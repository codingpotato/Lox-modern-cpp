#include <doctest/doctest.h>

#include "list.h"
#include "object.h"

struct Node {
  int value = 0;
  Node* next = nullptr;
};

TEST_CASE("list") {
  lox::List<Node> list;
  REQUIRE(list.empty());

  constexpr int count = 10;
  for (auto i = 0; i < count; ++i) {
    list.insert(new Node{i});
  }

  SUBCASE("iterator") {
    auto expected = count - 1;
    for (auto it = list.begin(); it != list.end(); ++it) {
      REQUIRE_EQ(it->value, expected);
      --expected;
    }
  }
  SUBCASE("erase if") {
    list.erase_if([](auto node) { return node->value % 2 == 0; });
    auto expected = count - 1;
    for (auto it = list.begin(); it != list.end(); ++it) {
      REQUIRE_EQ(it->value, expected);
      expected -= 2;
    }
  }
}

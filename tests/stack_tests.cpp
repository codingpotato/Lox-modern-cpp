#include <doctest/doctest.h>

#include "stack.h"

TEST_CASE("stack") {
  lox::Stack<int, 3> stack{};
  REQUIRE(stack.empty());
  REQUIRE_EQ(stack.size(), 0);

  stack.push(1);
  stack.push(2);
  stack.push(3);
  REQUIRE(!stack.empty());
  REQUIRE_EQ(stack.size(), 3);
  REQUIRE_EQ(stack.peek(), 3);
  REQUIRE_EQ(stack.peek(1), 2);
  REQUIRE_EQ(stack.peek(2), 1);

  REQUIRE_EQ(stack.pop(), 3);
  REQUIRE_EQ(stack.pop(), 2);
  REQUIRE_EQ(stack.pop(), 1);
}

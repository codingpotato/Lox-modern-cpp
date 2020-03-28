#include <doctest/doctest.h>

#include "list.h"
#include "object.h"

TEST_CASE("list") {
  lox::list list;
  constexpr int count = 10;
  for (int i = 0; i < count; ++i) {
    list.insert(new lox::string{std::to_string(i)});
  }
  int expected = count - 1;
  for (auto it = list.begin(); it != list.end(); ++it) {
    CHECK_EQ(it->as<lox::string>(), std::to_string(expected));
    --expected;
  }
}

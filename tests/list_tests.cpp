#include <doctest/doctest.h>

#include "list.h"
#include "object.h"

TEST_CASE("list") {
  lox::List<lox::Object> strings;
  constexpr int count = 10;
  for (auto i = 0; i < count; ++i) {
    strings.insert(new lox::String{std::to_string(i)});
  }
  auto expected = count - 1;
  for (auto it = strings.begin(); it != strings.end(); ++it) {
    CHECK_EQ(*it->as<lox::String>(), std::to_string(expected));
    --expected;
  }
}

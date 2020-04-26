#include <doctest/doctest.h>

#include "list.h"
#include "object.h"

TEST_CASE("list") {
  lox::List<lox::Object> strings;
  REQUIRE(strings.empty());

  constexpr int count = 10;
  for (auto i = 0; i < count; ++i) {
    strings.insert(new lox::String{std::to_string(i)});
  }

  SUBCASE("iterator") {
    auto expected = count - 1;
    for (auto it = strings.begin(); it != strings.end(); ++it) {
      REQUIRE_EQ(*it->as<lox::String>(), std::to_string(expected));
      --expected;
    }
  }
  SUBCASE("erase if") {
    strings.erase_if([](auto object) {
      return std::stoi(object->template as<lox::String>()->get_string()) % 2 ==
             0;
    });
    auto expected = count - 1;
    for (auto it = strings.begin(); it != strings.end(); ++it) {
      REQUIRE_EQ(*it->as<lox::String>(), std::to_string(expected));
      expected -= 2;
    }
  }
}

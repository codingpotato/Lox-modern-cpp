#include <doctest/doctest.h>

#include <string>

#include "object.h"

TEST_CASE("object") {
  const std::string str = "string";
  lox::String string{str};
  REQUIRE_EQ(string.get_hash(), lox::String::hash_from(str));
  REQUIRE_EQ(string, str);
}

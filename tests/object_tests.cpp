#include <doctest/doctest.h>

#include <string>

#include "object.h"

TEST_CASE("string") {
  const std::string str = "test string";
  lox::String string{str};
  CHECK_EQ(string.get_hash(), lox::String::hash_from(str));
  CHECK_EQ(string, str);
}

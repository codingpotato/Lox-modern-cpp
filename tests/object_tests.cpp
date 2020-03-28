#include <doctest/doctest.h>

#include <ostream>
#include <string>

#include "object.h"

TEST_CASE("string") {
  const std::string str = "test string";
  lox::string string{str};
  CHECK_EQ(string.hash(), lox::string::hash(str));
  CHECK_EQ(string, str);
}

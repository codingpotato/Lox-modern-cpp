#include <doctest/doctest.h>

#include "object.h"

TEST_CASE("object") {
  const lox::string str{"test string"};
  lox::object object{str};
  CHECK_EQ(object.as<lox::string>(), str);
}

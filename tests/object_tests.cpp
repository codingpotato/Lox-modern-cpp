#include <doctest.h>

#include "object.h"

TEST_CASE("object") {
  const std::string str{"test string"};
  lox::object object{str};
  CHECK_EQ(object.as<std::string>(), str);
}

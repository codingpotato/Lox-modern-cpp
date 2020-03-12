#include <doctest.h>

#include "value.h"

TEST_CASE("value") {
  const auto str = "test string";
  const lox::value v{str};
  CHECK(v.is<std::string>());
  CHECK_EQ(v.as<std::string>(), str);
}

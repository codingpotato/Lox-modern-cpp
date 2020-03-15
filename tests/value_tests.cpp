#include <doctest.h>

#include "value.h"

TEST_CASE("value") {
  const auto str = "test string";
  const lox::value v{str};
  CHECK(v.is<std::string>());
  CHECK_EQ(v.as<std::string>(), str);
}

TEST_CASE("copy string value") {
  const auto str = "test string";
  const lox::value v1{str};
  const lox::value v2{v1};
  CHECK(v1.is<std::string>());
  CHECK_EQ(v1.as<std::string>(), str);
  CHECK(v2.is<std::string>());
  CHECK_EQ(v2.as<std::string>(), str);
}

TEST_CASE("move string value") {
  const auto str = "test string";
  lox::value v1{str};
  const lox::value v2{std::move(v1)};
  CHECK(v1.is<lox::nil>());
  CHECK(v2.is<std::string>());
  CHECK_EQ(v2.as<std::string>(), str);
}

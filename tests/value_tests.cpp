#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "value.h"

TEST_CASE("arithmetic operator", "[value]") {
  lox::value v1{1};
  lox::value v2{2};
  lox::value v = v1 + v2;
  CHECK(v.is_type<int>());
  CHECK(v.as<int>() == 3);
}

TEST_CASE("logic operator", "[value]") {
  lox::value v1{1};
  lox::value v2{2};
  lox::value v = v1 < v2;
  CHECK(v.is_type<bool>());
  CHECK(v.as<bool>() == true);
}
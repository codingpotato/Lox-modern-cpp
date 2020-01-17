#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "value.h"

TEST_CASE("value", "[value]") {
  lox::value v1{1};
  lox::value v2{1};
  lox::value v = v1 + v2;
  CHECK(v.is_type<int>());
  CHECK(v.as<int>() == 2);
}
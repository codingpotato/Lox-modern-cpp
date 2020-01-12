#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "expression.h"

TEST_CASE("size of expression", "[expression]") {
  CHECK(sizeof(lox::expression::assignment) == 4);
  CHECK(sizeof(lox::expression::binary) == 4);
  CHECK(sizeof(lox::expression::call) == 1);
  CHECK(sizeof(lox::expression::group) == 1);
  CHECK(sizeof(lox::expression::literal) == sizeof(double) + sizeof(long));
  CHECK(sizeof(lox::expression::unary) == 4);
  CHECK(sizeof(lox::expression::variable) == 4);
  CHECK(sizeof(lox::expression::element_t) ==
        sizeof(lox::expression::literal) + sizeof(long));
  CHECK(sizeof(lox::expression) == 40);
}

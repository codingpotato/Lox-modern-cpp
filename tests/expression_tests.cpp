#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "expression.h"

// not portable
TEST_CASE("size of expression", "[expression]") {
  CHECK(sizeof(lox::expression::assignment) == 4);
  CHECK(sizeof(lox::expression::binary) == 4);
  CHECK(sizeof(lox::expression::call) == 1);
  CHECK(sizeof(lox::expression::group) == 1);
  CHECK(sizeof(lox::expression::literal) == 8);
  CHECK(sizeof(lox::expression::unary) == 4);
  CHECK(sizeof(lox::expression::variable) == 4);
  CHECK(sizeof(lox::expression::element_t) == 12);
  CHECK(sizeof(lox::expression) == 16);
}

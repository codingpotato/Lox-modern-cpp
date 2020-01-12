#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "statement.h"

TEST_CASE("size of statement", "[statements]") {
  CHECK(sizeof(lox::statement::block) == 8);
  CHECK(sizeof(lox::statement::expression_s) == 4);
  CHECK(sizeof(lox::statement::for_s) == 12);
  CHECK(sizeof(lox::statement::function) == 12);
  CHECK(sizeof(lox::statement::if_else) == 4);
  CHECK(sizeof(lox::statement::return_s) == 4);
  CHECK(sizeof(lox::statement::variable_s) == 8);
  CHECK(sizeof(lox::statement::while_s) == 4);
  CHECK(sizeof(lox::statement) == 16);
}

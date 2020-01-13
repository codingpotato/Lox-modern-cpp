#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "statement.h"

TEST_CASE("add string", "[program]") {
  lox::program program;
  REQUIRE(program.string_literals.add("string") == 0);
}

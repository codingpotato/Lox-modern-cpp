#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "parser.h"
#include "scanner.h"
#include "to_string.h"

TEST_CASE("binary expression", "[to_string]") {
  lox::scanner scanner{"1 + 2;"};
  lox::parser parser{};
  auto program = parser.parse(scanner.scan());
  REQUIRE(lox::to_string(program) == "(1 + 2)");
}

TEST_CASE("multiple binary expression", "[to_string]") {
  lox::scanner scanner{"1 + 2 * 3 - 4;"};
  lox::parser parser{};
  auto program = parser.parse(scanner.scan());
  REQUIRE(lox::to_string(program) == "((1 + (2 * 3)) - 4)");
}

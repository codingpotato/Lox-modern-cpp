#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "parser.h"
#include "program.h"
#include "scanner.h"

TEST_CASE("parse primary", "[parser]") {
  lox::scanner scanner{"1 + 2;"};
  lox::parser parser{};
  auto program = parser.parse(scanner.scan());
  REQUIRE(program.expressions.size() == 3);
  REQUIRE(program.statements.size() == 2);
}

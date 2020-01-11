#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <iostream>

#include "parser.h"
#include "program.h"
#include "scanner.h"

TEST_CASE("parse primary", "[parser]") {
  lox::scanner scanner{"1 + 2"};
  lox::parser parser{scanner.scan()};
  lox::program program;
  parser.parse_addition(program);
  REQUIRE(sizeof(lox::expression::element) == 4);
  REQUIRE(program.expressions.size() == 3);
}
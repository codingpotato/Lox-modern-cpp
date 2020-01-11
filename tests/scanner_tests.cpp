#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "scanner.h"

TEST_CASE("scan", "[scanner]") {
  lox::scanner scanner("1 + 2");
  auto tokens = scanner.scan();
  REQUIRE(tokens.size() == 4);
  REQUIRE(tokens[0].type == lox::token::number);
  REQUIRE(std::get<int>(tokens[0].value.value) == 1);
  REQUIRE(tokens[1].type == lox::token::plus);
  REQUIRE(tokens[2].type == lox::token::number);
  REQUIRE(std::get<int>(tokens[2].value.value) == 2);
  REQUIRE(tokens[3].type == lox::token::eof);
}

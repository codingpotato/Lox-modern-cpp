#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "parser.h"
#include "program.h"
#include "scanner.h"
#include "to_string.h"

lox::program parse(const lox::string& source) noexcept {
  lox::scanner scanner{source};
  lox::parser parser{};
  return parser.parse(scanner.scan());
}

TEST_CASE("parse block statement", "[parser]") {
  lox::string source{"1 + 2;"};
  lox::string expected = R"({
  1 + 2;
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse variable declearation statement", "[parser]") {
  lox::string source{"var name = 1;"};
  lox::string expected = R"({
  var name = 1;
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse if else statement", "[parser]") {
  lox::string source{
      R"(if (1 == 1) {
  a = 1;
} else {
  a = 2;
})"};
  lox::string expected = R"({
  if (1 == 1)
  {
    a = 1;
  }
  else
  {
    a = 2;
  }
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse group expression", "[parser]") {
  lox::string source{"(1 + 2) * 3;"};
  lox::string expected = R"({
  (1 + 2) * 3;
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

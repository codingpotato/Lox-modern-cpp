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

TEST_CASE("parse for statement", "[parser]") {
  lox::string source{R"(
for (var i = 0; i < 100; i = i + 1) {
  a = 1;
}
)"};
  lox::string expected = R"({
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse if else statement", "[parser]") {
  lox::string source{
      R"(
if (1 == 1) {
  a = 1;
  b = 2;
} else {
  c = 3;
  d = 4;
}
)"};
  lox::string expected = R"({
  if (1 == 1)
    {
      a = 1;
      b = 2;
    }
  else
    {
      c = 3;
      d = 4;
    }
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse if else with single statement", "[parser]") {
  lox::string source{
      R"(
if (1 == 1)
  a = 1;
else 
  c = 3;
)"};
  lox::string expected = R"({
  if (1 == 1)
    a = 1;
  else
    c = 3;
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

TEST_CASE("parse while statement", "[parser]") {
  lox::string source{
      R"(while (1 == 1) {
  a = 1;
  b = 2;
})"};
  lox::string expected = R"({
  while (1 == 1)
    {
      a = 1;
      b = 2;
    }
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse while with single statement", "[parser]") {
  lox::string source{
      R"(
while (1 == 1) a = 1;
)"};
  lox::string expected = R"({
  while (1 == 1)
    a = 1;
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse assignment expression", "[parser]") {
  lox::string source{"i = i + 1;"};
  lox::string expected = R"({
  i = i + 1;
}
)";
  REQUIRE(lox::to_string(parse(source)) == expected);
}

TEST_CASE("parse binary expression", "[parser]") {
  lox::string source{"1 + 2 * 3 - 4;"};
  lox::string expected = R"({
  1 + 2 * 3 - 4;
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

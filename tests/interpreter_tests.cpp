#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sstream>

#include "exception.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "types.h"

static lox::string execute(lox::string source) noexcept {
  lox::scanner scanner{std::move(source)};
  lox::parser parser{};
  auto program = parser.parse(scanner.scan());
  std::stringstream stream;
  lox::interpreter interpreter{stream};
  interpreter.execute(program);
  return stream.str();
}

TEST_CASE("execute if else statement", "[interpreter]") {
  lox::string source{R"(
  var a = 0;
  if (a > 0) {
    a = a + 1;
  } else {
    a = a - 1;
  }
  print a;
)"};
  REQUIRE(execute(source) == "-1");
}

TEST_CASE("execute if else with single statement", "[interpreter]") {
  REQUIRE(execute("if (true) print 1; else print 2;") == "1");
  REQUIRE(execute("if (false) print 1; else print 2;") == "2");
}

TEST_CASE("execute while statement", "[interpreter]") {
  lox::string source{R"(
  var sum = 0;
  var i = 1;
  while (i <= 100) {
    sum = sum + i;
    i = i + 1;
  }
  print sum;
)"};
  REQUIRE(execute(source) == "5050");
}

TEST_CASE("execute varibale declearation", "[interpreter]") {
  lox::string source{R"(
  var a = 100;
  print a;
)"};
  REQUIRE(execute(source) == "100");
}

TEST_CASE("execute arithmetic binary expression", "[interpreter]") {
  lox::string source{"print 1 + 2;"};
  REQUIRE(execute(source) == "3");
}

TEST_CASE("execute logic binary expression", "[interpreter]") {
  REQUIRE(execute("print 1 < 2;") == "true");
  REQUIRE(execute("print 1 > 2;") == "false");
}

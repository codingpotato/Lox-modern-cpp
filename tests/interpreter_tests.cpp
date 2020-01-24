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

/*TEST_CASE("execute block", "[interpreter]") {
  lox::string source{R"(
  var sum = 0;
  var i = 0;
  while (i < 100) {
    sum = sum + i;
    i = i + 1;
  }
  print sum;
)"};
  REQUIRE(execute(source) == "0");
}*/

TEST_CASE("execute varibale declearation", "[interpreter]") {
  lox::string source{R"(
  var a = 100;
  print a;
)"};
  REQUIRE(execute(source) == "100");
}

TEST_CASE("execute expression", "[interpreter]") {
  lox::string source{"print 1 + 2;"};
  REQUIRE(execute(source) == "3");
}

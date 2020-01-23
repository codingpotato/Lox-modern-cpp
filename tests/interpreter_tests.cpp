#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sstream>

#include "exception.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"

TEST_CASE("execute expression", "[interpreter]") {
  lox::scanner scanner{"print 1 + 2;"};
  lox::parser parser{};
  auto program = parser.parse(scanner.scan());
  std::stringstream str;
  lox::interpreter interpreter{str};
  interpreter.execute(program);
  REQUIRE(str.str() == "3");
}

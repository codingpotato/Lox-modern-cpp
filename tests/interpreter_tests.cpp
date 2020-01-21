#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "exception.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"

TEST_CASE("execute expression", "[interpreter]") {
  lox::scanner scanner{"1 + 2;"};
  lox::parser parser{};
  auto program = parser.parse(scanner.scan());
  lox::interpreter interpreter{};
  interpreter.execute(program);
}

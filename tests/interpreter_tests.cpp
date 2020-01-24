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

TEST_CASE("execute expression", "[interpreter]") {
  lox::string source{"print 1 + 2;"};
  REQUIRE(execute(source) == "3");
}

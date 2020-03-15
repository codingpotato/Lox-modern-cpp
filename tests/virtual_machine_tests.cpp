#include <doctest.h>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

TEST_CASE("run") {
  lox::scanner scanner{"print 1 + 2;"};
  auto chunk = lox::compiler{}.compile(scanner.scan());
  lox::virtual_machine{}.interpret(chunk);
}

TEST_CASE("global variable") {
  lox::scanner scanner{"var a = 1; var b = 2; print a + b;"};
  auto chunk = lox::compiler{}.compile(scanner.scan());
  lox::virtual_machine{}.interpret(chunk);
}

TEST_CASE("set global variable") {
  lox::scanner scanner{"var a = 1; a = 2; print a;"};
  auto chunk = lox::compiler{}.compile(scanner.scan());
  lox::virtual_machine{}.interpret(chunk);
}

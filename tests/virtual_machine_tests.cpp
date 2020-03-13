#include <doctest.h>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

TEST_CASE("run") {
  lox::scanner scanner{"print 1 + 2;"};
  auto chunk = lox::compiler{}.compile(scanner.scan());
  chunk.add_instruction(lox::op_return{}, 2);
  lox::virtual_machine{}.interpret(chunk);
}

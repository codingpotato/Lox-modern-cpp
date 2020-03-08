#include <doctest.h>

#include "virtual_machine.h"

TEST_CASE("") {
  lox::chunk main;
  auto constant = main.add_constant(1.2);
  main.add_instruction(lox::instruction::op_constant, constant, 123);
  main.add_instruction(lox::instruction::op_negate, 123);
  main.add_instruction(lox::instruction::op_return, 123);
  lox::virtual_machine vm;
  vm.interpret(main);
}

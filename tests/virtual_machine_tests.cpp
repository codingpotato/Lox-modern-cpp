#include <doctest.h>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

TEST_CASE("") {
  lox::chunk main;
  auto constant = main.add_constant(1.2);
  main.add_instruction(lox::op_constant{}, constant, 123);

  constant = main.add_constant(3.4);
  main.add_instruction(lox::op_constant{}, constant, 123);

  main.add_instruction(lox::op_add{}, 123);

  constant = main.add_constant(5.6);
  main.add_instruction(lox::op_constant{}, constant, 123);

  main.add_instruction(lox::op_divide{}, 123);

  main.add_instruction(lox::op_return{}, 123);

  lox::virtual_machine vm;
  vm.interpret(main);
}

TEST_CASE("run") {
  lox::scanner scanner{"1 == 2"};
  auto chunk = lox::compiler{}.compile(scanner.scan());
  chunk.add_instruction(lox::op_return{}, 2);
  lox::virtual_machine{}.interpret(chunk);
}

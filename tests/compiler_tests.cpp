#include <doctest/doctest.h>

#include <ostream>
#include <string>

#include "compiler.h"
#include "virtual_machine.h"

TEST_CASE("compiler add expression") {
  lox::token_vector tokens{
      {lox::token::k_print, "print", 1}, {lox::token::number, "1", 1},
      {lox::token::plus, "+", 1},        {lox::token::number, "2", 1},
      {lox::token::semicolon, ";", 1},   {lox::token::eof, "", 1},
  };
  std::ostringstream oss;
  lox::virtual_machine vm{oss};
  lox::compiler{vm}.compile(tokens);
  const std::string expected = R"(== test expression ==
0000    1 op_constant 1
0001    | op_constant 2
0002    | op_add
0003    | op_print
)";
  CHECK_EQ(vm.main().repr("test expression"), expected);
}

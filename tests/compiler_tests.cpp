#include <doctest.h>

#include <string>

#include "compiler.h"

TEST_CASE("compiler") {
  lox::token_vector tokens{
      {lox::token::number, "1", 1},
      {lox::token::plus, "+", 1},
      {lox::token::number, "2", 1},
      {lox::token::eof, "", 1},
  };
  lox::compiler compiler;
  const auto chunk = compiler.compile(tokens);
  const std::string expected = R"(== test expression ==
0000    1 OP_CONSTANT 1
0001    | OP_CONSTANT 2
0002    | OP_ADD
)";
  CHECK_EQ(chunk.repr("test expression"), expected);
}

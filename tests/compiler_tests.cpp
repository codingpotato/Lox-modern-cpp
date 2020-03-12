#include <doctest.h>

#include <string>

#include "compiler.h"

TEST_CASE("compiler add expression") {
  lox::token_vector tokens{
      {lox::token::number, "1", 1},
      {lox::token::plus, "+", 1},
      {lox::token::number, "2", 1},
      {lox::token::eof, "", 1},
  };
  const auto chunk = lox::compiler{}.compile(tokens);
  const std::string expected = R"(== test expression ==
0000    1 op_constant 1
0001    | op_constant 2
0002    | op_add
)";
  CHECK_EQ(chunk.repr("test expression"), expected);
}

TEST_CASE("compiler not expression") {
  lox::token_vector tokens{
      {lox::token::bang, "!", 1},
      {lox::token::k_false, "", 1},
      {lox::token::eof, "", 1},
  };
  const auto chunk = lox::compiler{}.compile(tokens);
  const std::string expected = R"(== test expression ==
0000    1 op_false
0001    | op_not
)";
  CHECK_EQ(chunk.repr("test expression"), expected);
}

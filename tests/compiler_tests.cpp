#include <doctest.h>

#include "compiler.h"

TEST_CASE("compiler") {
  lox::token_vector tokens{
      {lox::token::number, "1", 1},
      {lox::token::plus, "+", 1},
      {lox::token::number, "2", 1},
      {lox::token::eof, "", 1},
  };
  lox::compiler compiler;
  auto chunk = compiler.compile(tokens);
  CHECK_EQ(chunk.code().size(), 3);
  CHECK_EQ(chunk.constants().size(), 2);
}

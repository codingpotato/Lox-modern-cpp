#include <doctest.h>

#include "compiler.h"

TEST_CASE("") {
  lox::compiler c;
  lox::token_vector tokens;
  tokens.emplace_back(lox::token::number, "1", 1);
  tokens.emplace_back(lox::token::eof, "", 1);
  c.compile(tokens);
}

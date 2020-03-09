#include <doctest.h>

#include "compiler.h"

TEST_CASE("compiler") {
  lox::compiler c;
  lox::token_vector tokens;
  tokens.emplace_back(lox::token::number, "1", 1);
  tokens.emplace_back(lox::token::plus, "+", 1);
  tokens.emplace_back(lox::token::number, "2", 1);
  tokens.emplace_back(lox::token::semicolon, ";", 1);
  tokens.emplace_back(lox::token::eof, "", 1);
  c.compile(tokens);
}

#include <doctest.h>

#include "scanner.h"

inline void check_tokens(const lox::token_vector& tokens,
                         const lox::token_vector& expected) noexcept {
  (void)(expected);
  CHECK_EQ(tokens.size(), expected.size());
  for (std::size_t i = 0; i < tokens.size(); ++i) {
    CHECK_EQ(tokens[i].type, expected[i].type);
    CHECK_EQ(tokens[i].lexeme, expected[i].lexeme);
    CHECK_EQ(tokens[i].line, expected[i].line);
  }
}

TEST_CASE("scan") {
  lox::scanner scanner{"print 1 + 2;"};
  auto tokens = scanner.scan();
  std::vector<lox::token> expected = {
      {lox::token::k_print, "print", 1}, {lox::token::number, "1", 1},
      {lox::token::plus, "+", 1},        {lox::token::number, "2", 1},
      {lox::token::semicolon, ";", 1},   {lox::token::eof, "", 1},
  };
  check_tokens(tokens, expected);
}

TEST_CASE("scan if statement") {
  lox::scanner scanner{"if (true) print 1;"};
  auto tokens = scanner.scan();
  std::vector<lox::token> expected = {
      {lox::token::k_if, "if", 1},       {lox::token::left_paren, "(", 1},
      {lox::token::k_true, "true", 1},   {lox::token::right_paren, ")", 1},
      {lox::token::k_print, "print", 1}, {lox::token::number, "1", 1},
      {lox::token::semicolon, ";", 1},   {lox::token::eof, "", 1},
  };
  check_tokens(tokens, expected);
}

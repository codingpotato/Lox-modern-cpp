#include <doctest/doctest.h>

#include "scanner.h"

inline void check_tokens(const lox::Token_vector& tokens,
                         const lox::Token_vector& expected) noexcept {
  (void)(expected);
  CHECK_EQ(tokens.size(), expected.size());
  for (std::size_t i = 0; i < tokens.size(); ++i) {
    CHECK_EQ(tokens[i].type, expected[i].type);
    CHECK_EQ(tokens[i].lexeme, expected[i].lexeme);
    CHECK_EQ(tokens[i].line, expected[i].line);
  }
}

TEST_CASE("scan") {
  lox::Scanner scanner{"print 1 + 2;"};
  auto tokens = scanner.scan();
  std::vector<lox::Token> expected = {
      {lox::Token::k_print, "print", 1}, {lox::Token::number, "1", 1},
      {lox::Token::plus, "+", 1},        {lox::Token::number, "2", 1},
      {lox::Token::semicolon, ";", 1},   {lox::Token::eof, "", 1},
  };
  check_tokens(tokens, expected);
}

TEST_CASE("scan if statement") {
  lox::Scanner scanner{"if (true) print 1;"};
  auto tokens = scanner.scan();
  std::vector<lox::Token> expected = {
      {lox::Token::k_if, "if", 1},       {lox::Token::left_paren, "(", 1},
      {lox::Token::k_true, "true", 1},   {lox::Token::right_paren, ")", 1},
      {lox::Token::k_print, "print", 1}, {lox::Token::number, "1", 1},
      {lox::Token::semicolon, ";", 1},   {lox::Token::eof, "", 1},
  };
  check_tokens(tokens, expected);
}

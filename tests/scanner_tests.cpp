#include <doctest/doctest.h>

#include "helper.h"
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

TEST_CASE("scanner: identifiers") {
  lox::Scanner scanner{R"(
andy formless fo _ _123 _abc ab123
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_
)"};
  auto tokens = scanner.scan();
  std::vector<lox::Token> expected = {
      {lox::Token::identifier, "andy", 2},
      {lox::Token::identifier, "formless", 2},
      {lox::Token::identifier, "fo", 2},
      {lox::Token::identifier, "_", 2},
      {lox::Token::identifier, "_123", 2},
      {lox::Token::identifier, "_abc", 2},
      {lox::Token::identifier, "ab123", 2},
      {lox::Token::identifier,
       "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_", 3},
      {lox::Token::eof, "", 4},
  };
  check_tokens(tokens, expected);
}

TEST_CASE("scanner: keywords") {
  lox::Scanner scanner{R"(
and class else false for fun if nil or return super this true var while
)"};
  auto tokens = scanner.scan();
  std::vector<lox::Token> expected = {
      {lox::Token::k_and, "and", 2},     {lox::Token::k_class, "class", 2},
      {lox::Token::k_else, "else", 2},   {lox::Token::k_false, "false", 2},
      {lox::Token::k_for, "for", 2},     {lox::Token::k_func, "fun", 2},
      {lox::Token::k_if, "if", 2},       {lox::Token::k_nil, "nil", 2},
      {lox::Token::k_or, "or", 2},       {lox::Token::k_return, "return", 2},
      {lox::Token::k_super, "super", 2}, {lox::Token::k_this, "this", 2},
      {lox::Token::k_true, "true", 2},   {lox::Token::k_var, "var", 2},
      {lox::Token::k_while, "while", 2}, {lox::Token::eof, "", 3},
  };
  check_tokens(tokens, expected);
}

TEST_CASE("scanner: numbers") {
  lox::Scanner scanner{R"(
123
123.456
.456
123.
)"};
  auto tokens = scanner.scan();
  std::vector<lox::Token> expected = {
      {lox::Token::number, "123", 2}, {lox::Token::number, "123.456", 3},
      {lox::Token::dot, ".", 4},      {lox::Token::number, "456", 4},
      {lox::Token::number, "123", 5}, {lox::Token::dot, ".", 5},
      {lox::Token::eof, "", 6},
  };
  check_tokens(tokens, expected);
}

TEST_CASE("scanner: empty") {
  const std::string source{""};
  const std::string expected{""};
  REQUIRE_EQ(run(source), expected);
}

TEST_CASE("scanner: unexpected character") {
  const std::string source{R"(
foo(a | b);
)"};
  const std::string expected{R"([line 2] Error at '|': Unexpected character.
)"};
  REQUIRE_EQ(run(source), expected);
}

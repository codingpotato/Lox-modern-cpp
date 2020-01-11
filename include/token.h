#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include <string>
#include <variant>
#include <vector>

namespace lox {

struct token {
  enum type_t {
    // Single-character tokens
    left_paren,
    right_paren,
    left_brace,
    right_brace,
    comma,
    dot,
    minus,
    plus,
    semicolon,
    slash,
    star,

    // One or two character tokens
    bang,
    bang_equal,
    equal,
    equal_equal,
    greater,
    greater_equal,
    less,
    less_equal,

    // Literals
    identifier,
    string,
    number,

    // Keywords
    k_and,
    k_class,
    k_else,
    k_false,
    k_fun,
    k_for,
    k_if,
    k_nil,
    k_or,
    k_print,
    k_return,
    k_super,
    k_this,
    k_true,
    k_var,
    k_while,

    eof
  };

  struct literal {
    std::variant<int, double, std::string> value;
  };

  explicit token(type_t t) noexcept : type{t} {}

  token(type_t t, int int_value) noexcept : type{t}, value{int_value} {}
  token(type_t t, double double_value) noexcept
      : type{t}, value{double_value} {}
  token(type_t t, std::string string_value) noexcept
      : type{t}, value{string_value} {}

  token(type_t t, std::string l, int li) noexcept
      : type{t}, lexeme{std::move(l)}, line{li} {}

  type_t type;
  std::string lexeme;
  literal value;
  int line;
};

using token_vector = std::vector<token>;

} // namespace lox

#endif

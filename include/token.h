#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include <string>
#include <variant>
#include <vector>

#include "types.h"
#include "variant_storage.h"

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
    l_identifier,
    l_string,
    l_number,

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
    variant_storage<std::variant<int, double, string>> storage;
  };

  explicit token(type_t t, int li) noexcept : type{t}, line{li} {}

  template <typename T>
  token(type_t t, T v, int li) noexcept
      : type{t}, value{std::move(v)}, line{li} {}

  type_t type;
  literal value;
  int line;
};

using token_vector = std::vector<token>;

}  // namespace lox

#endif

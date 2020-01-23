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

inline string to_string(const token& t) noexcept {
  string output = std::to_string(t.line) + ": ";
  switch (t.type) {
    case token::left_paren:
      output += "(";
      break;
    case token::right_paren:
      output += ")";
      break;
    case token::left_brace:
      output += "{";
      break;
    case token::right_brace:
      output += "}";
      break;
    case token::comma:
      output += ",";
      break;
    case token::dot:
      output += ".";
      break;
    case token::minus:
      output += "-";
      break;
    case token::plus:
      output += "+";
      break;
    case token::semicolon:
      output += ";";
      break;
    case token::slash:
      output += "/";
      break;
    case token::star:
      output += "*";
      break;
    case token::bang:
      output += "!";
      break;
    case token::bang_equal:
      output += "!=";
      break;
    case token::equal:
      output += "=";
      break;
    case token::equal_equal:
      output += "==";
      break;
    case token::greater:
      output += ">";
      break;
    case token::greater_equal:
      output += ">=";
      break;
    case token::less:
      output += "<";
      break;
    case token::less_equal:
      output += "<=";
      break;
    case token::l_identifier:
    case token::l_string:
      output += t.value.storage.as<string>();
      break;
    case token::l_number:
      output += std::to_string(t.value.storage.is_type<int>()
                                   ? t.value.storage.as<int>()
                                   : t.value.storage.as<double>());
      break;
    case token::k_and:
      output += "and";
      break;
    case token::k_class:
      output += "class";
      break;
    case token::k_else:
      output += "else";
      break;
    case token::k_false:
      output += "false";
      break;
    case token::k_fun:
      output += "fun";
      break;
    case token::k_for:
      output += "for";
      break;
    case token::k_if:
      output += "if";
      break;
    case token::k_nil:
      output += "nil";
      break;
    case token::k_or:
      output += "or";
      break;
    case token::k_print:
      output += "print";
      break;
    case token::k_return:
      output += "output +=";
      break;
    case token::k_super:
      output += "super";
      break;
    case token::k_this:
      output += "this";
      break;
    case token::k_true:
      output += "true";
      break;
    case token::k_var:
      output += "var";
      break;
    case token::k_while:
      output += "while";
      break;
    case token::eof:
      output += "eof";
      break;
  }
  return output;
}

}  // namespace lox

#endif

#ifndef SCANNER_H
#define SCANNER_H

#include <map>
#include <string>

namespace lox {

struct token {
  enum type {
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

  type t;
};

constexpr bool operator==(const token &lhs, const token &rhs) noexcept {
  return lhs.t == rhs.t;
}

constexpr bool is_at_end(std::string::const_iterator it,
                         const std::string &source) noexcept {
  return it == source.cend();
}

template <typename Iterator>
constexpr bool match(Iterator &it, char expected) noexcept {
  if (*it != expected) {
    return false;
  }
  ++it;
  return true;
}

template <typename Iterator> token scan_token(Iterator &it) {
  static const std::map<char, token::type> char_token_type_map{
      {'(', token::left_paren}, {')', token::right_paren},
      {'{', token::left_brace}, {'}', token::right_brace},
      {',', token::comma},      {'.', token::dot},
      {'-', token::minus},      {'+', token::plus},
      {';', token::semicolon},  {'*', token::star},
  };
  const auto ch = *it;
  ++it;
  if (char_token_type_map.find(ch) != char_token_type_map.end()) {
    return token{char_token_type_map.at(ch)};
  }
  throw std::exception{};
}

inline std::vector<token> scan(const std::string &source) {
  std::vector<token> tokens;
  auto it = source.cbegin();
  while (!is_at_end(it, source)) {
    tokens.push_back(scan_token(it));
  }
  return tokens;
}

} // namespace lox

#endif
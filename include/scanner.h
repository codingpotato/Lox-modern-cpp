#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include <map>
#include <string>
#include <tuple>
#include <utility>
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

  type_t type;
};

using token_vector = std::vector<token>;

constexpr bool operator==(const token &lhs, const token &rhs) noexcept {
  return lhs.type == rhs.type;
}

template <typename Iterator>
std::tuple<bool, Iterator> match(Iterator first, Iterator last,
                                 char expected) noexcept {
  if (first == last || *first != expected) {
    return {false, first};
  }
  return {true, first + 1};
}

template <typename Iterator>
std::tuple<token, Iterator> scan_token(Iterator first, Iterator) {
  static const std::map<char, token::type_t> char_token_type_map{
      {'(', token::left_paren}, {')', token::right_paren},
      {'{', token::left_brace}, {'}', token::right_brace},
      {',', token::comma},      {'.', token::dot},
      {'-', token::minus},      {'+', token::plus},
      {';', token::semicolon},  {'*', token::star},
  };
  const auto ch = *first;
  if (char_token_type_map.find(ch) != char_token_type_map.end()) {
    return {token{char_token_type_map.at(ch)}, first + 1};
  }
  throw std::exception{};
}

inline token_vector scan(const std::string &source) {
  std::vector<token> tokens;
  auto first = source.cbegin();
  const auto last = source.cend();
  while (first != last) {
    auto [result_token, it] = scan_token(first, last);
    tokens.emplace_back(std::move(result_token));
    first = it;
  }
  return tokens;
}

}  // namespace lox

#endif
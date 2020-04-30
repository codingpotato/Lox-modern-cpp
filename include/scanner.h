#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include <string>
#include <vector>

#include "contract.h"
#include "exception.h"

namespace lox {

struct token {
  enum type_t {
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

    // one or two character tokens
    bang,
    bang_equal,
    equal,
    equal_equal,
    greater,
    greater_equal,
    less,
    less_equal,

    // literals
    identifier,
    number,
    string,

    // keywords
    k_and,
    k_class,
    k_else,
    k_false,
    k_for,
    k_func,
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

  token(type_t type, int line) noexcept : type{type}, line{line} {}
  token(type_t type, std::string lexeme, int line) noexcept
      : type{type}, lexeme{std::move(lexeme)}, line{line} {}

  type_t type;
  std::string lexeme;
  int line;
};

using token_vector = std::vector<token>;

inline bool is_alpha(char ch) noexcept {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

inline bool is_digit(char ch) noexcept { return ch >= '0' && ch <= '9'; }

struct scanner {
  explicit scanner(std::string source) noexcept : source{std::move(source)} {}

  token_vector scan() noexcept {
    current = source.cbegin();
    line = 1;
    token_vector tokens;
    while (true) {
      tokens.emplace_back(scan_token());
      if (tokens.back().type == token::eof) {
        break;
      }
    }
    return tokens;
  }

 private:
  token scan_token() {
    skip_white_space();
    start = current;
    if (is_at_end()) {
      return make_token(token::eof);
    }
    const auto ch = advance();
    if (is_alpha(ch)) {
      return identifier();
    }
    if (is_digit(ch)) {
      return number();
    }
    switch (ch) {
      case '(':
        return make_token(token::left_paren);
      case ')':
        return make_token(token::right_paren);
      case '{':
        return make_token(token::left_brace);
      case '}':
        return make_token(token::right_brace);
      case ';':
        return make_token(token::semicolon);
      case ',':
        return make_token(token::comma);
      case '.':
        return make_token(token::dot);
      case '-':
        return make_token(token::minus);
      case '+':
        return make_token(token::plus);
      case '/':
        return make_token(token::slash);
      case '*':
        return make_token(token::star);
      case '!':
        return make_token(match('=') ? token::bang_equal : token::bang);
      case '=':
        return make_token(match('=') ? token::equal_equal : token::equal);
      case '<':
        return make_token(match('=') ? token::less_equal : token::less);
      case '>':
        return make_token(match('=') ? token::greater_equal : token::greater);
      case '"':
        return string();
    }
    throw scan_error{"Unexpected character."};
  }

  token identifier() noexcept {
    while (!is_at_end() && (is_alpha(peek()) || is_digit(peek()))) {
      advance();
    }
    return make_token(identifier_type());
  }

  token number() noexcept {
    while (!is_at_end() && is_digit(peek())) {
      advance();
    }
    if (!is_at_end() && peek() == '.' && is_digit(peek_next())) {
      advance();
      while (!is_at_end() && is_digit(peek())) {
        advance();
      }
    }
    return make_token(token::number);
  }

  token string() {
    while (!is_at_end() && peek() != '"') {
      if (peek() == '\n') {
        ++line;
      }
      advance();
    }
    if (!is_at_end()) {
      advance();
      return make_token(token::string);
    }
    throw scan_error{"Unterminated string."};
  }

  token make_token(token::type_t type) const noexcept {
    if (type == token::string) {
      ENSURES(std::distance(start, current) >= 1);
      return {
          type, {start + 1, start + std::distance(start, current) - 1}, line};
    }
    return {type, {start, current}, line};
  }

  bool is_at_end() const noexcept { return current == source.cend(); }

  char advance() noexcept {
    ENSURES(!is_at_end());
    auto ch = *current;
    ++current;
    return ch;
  }

  bool match(char expected) noexcept {
    if (is_at_end() || *current != expected) {
      return false;
    }
    ++current;
    return true;
  }

  char peek() const noexcept {
    ENSURES(!is_at_end());
    return *current;
  }

  char peek_next() const noexcept {
    if (is_at_end()) {
      return '\0';
    } else {
      return *(current + 1);
    }
  }

  void skip_white_space() noexcept {
    while (!is_at_end()) {
      auto ch = peek();
      switch (ch) {
        case ' ':
        case '\r':
        case '\t':
          advance();
          break;
        case '\n':
          ++line;
          advance();
          break;
        case '/':
          if (peek_next() == '/') {
            while (!is_at_end() && peek() != '\n') {
              advance();
            }
          } else {
            return;
          }
          break;
        default:
          return;
      }
    }
  }

  token::type_t check_keyword(int index, const std::string& rest,
                              token::type_t type) const noexcept {
    auto first = start + index;
    auto last = start + index + rest.size();
    if (current == last &&
        std::equal(first, last, rest.cbegin(), rest.cend())) {
      return type;
    }
    return token::identifier;
  }

  token::type_t identifier_type() const noexcept {
    switch (*start) {
      case 'a':
        return check_keyword(1, "nd", token::k_and);
      case 'c':
        return check_keyword(1, "lass", token::k_class);
      case 'e':
        return check_keyword(1, "lse", token::k_else);
      case 'f':
        if (std::distance(start, source.cend()) > 1) {
          switch (*(start + 1)) {
            case 'a':
              return check_keyword(2, "lse", token::k_false);
            case 'o':
              return check_keyword(2, "r", token::k_for);
            case 'u':
              return check_keyword(2, "n", token::k_func);
          }
        }
        break;
      case 'i':
        return check_keyword(1, "f", token::k_if);
      case 'n':
        return check_keyword(1, "il", token::k_nil);
      case 'o':
        return check_keyword(1, "r", token::k_or);
      case 'p':
        return check_keyword(1, "rint", token::k_print);
      case 'r':
        return check_keyword(1, "eturn", token::k_return);
      case 's':
        return check_keyword(1, "uper", token::k_super);
      case 't':
        if (std::distance(start, source.cend()) > 1) {
          switch (*(start + 1)) {
            case 'h':
              return check_keyword(2, "is", token::k_this);
            case 'r':
              return check_keyword(2, "ue", token::k_true);
          }
        }
        break;
      case 'v':
        return check_keyword(1, "ar", token::k_var);
      case 'w':
        return check_keyword(1, "hile", token::k_while);
    }
    return token::identifier;
  }

  std::string source;
  std::string::const_iterator start;
  std::string::const_iterator current;
  int line;
};

}  // namespace lox

#endif

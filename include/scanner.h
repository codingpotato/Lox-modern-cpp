#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include <string>
#include <string_view>
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

  explicit token(type_t t, int l) noexcept : type{t}, line{l} {}
  token(type_t t, const char* string, std::size_t count, int l) noexcept
      : type{t}, lexeme{string, count}, line{l} {}

  type_t type;
  std::string_view lexeme;
  int line;
};

using token_vector = std::vector<token>;

inline bool is_alpha(char ch) noexcept {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

inline bool is_digit(char ch) noexcept { return ch >= '0' && ch <= '9'; }

struct scanner {
  explicit scanner(std::string source) noexcept : source_{std::move(source)} {}

  token_vector scan() noexcept {
    current_ = source_.cbegin();
    line_ = 0;
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
    start_ = current_;
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
    return make_token(token::identifier);
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
        ++line_;
      }
      advance();
    }
    if (is_at_end()) {
      throw scan_error{"Unterminated string."};
    }
    advance();
    return make_token(token::string);
  }

  token make_token(token::type_t type) const noexcept {
    return {type, source_.c_str() + std::distance(source_.cbegin(), start_),
            static_cast<std::size_t>(std::distance(start_, current_)), line_};
  }

  bool is_at_end() const noexcept { return current_ == source_.cend(); }

  char advance() noexcept {
    auto ch = *current_;
    ENSURES(!is_at_end());
    ++current_;
    return ch;
  }

  bool match(char expected) noexcept {
    if (is_at_end() || *current_ != expected) {
      return false;
    }
    ++current_;
    return true;
  }

  char peek() const noexcept {
    ENSURES(!is_at_end());
    return *current_;
  }

  char peek_next() const noexcept {
    if (is_at_end()) {
      return '\0';
    } else {
      return *(current_ + 1);
    }
  }

  void skip_white_space() noexcept {
    while (true) {
      auto ch = peek();
      switch (ch) {
        case ' ':
        case '\r':
        case '\t':
          advance();
          break;
        case '\n':
          ++line_;
          advance();
          break;
        case '/':
          if (peek_next() == '/') {
            while (peek() != '\n' && !is_at_end()) {
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

  token::type_t check_keyword(const std::string& rest, token::type_t type) const
      noexcept {
    if (std::equal(start_, source_.cend(), rest.cbegin(), rest.cend())) {
      return type;
    }
    return token::identifier;
  }

  token::type_t identifier_type() const noexcept {
    switch (*start_) {
      case 'a':
        return check_keyword("nd", token::k_and);
      case 'c':
        return check_keyword("lass", token::k_class);
      case 'e':
        return check_keyword("lse", token::k_else);
      case 'f':
        if (std::distance(start_, source_.cend()) > 1) {
          switch (*(start_ + 1)) {
            case 'a':
              return check_keyword("lse", token::k_false);
            case 'o':
              return check_keyword("r", token::k_for);
            case 'u':
              return check_keyword("n", token::k_func);
          }
        }
        break;
      case 'i':
        return check_keyword("f", token::k_if);
      case 'n':
        return check_keyword("il", token::k_nil);
      case 'o':
        return check_keyword("r", token::k_or);
      case 'p':
        return check_keyword("rint", token::k_print);
      case 'r':
        return check_keyword("eturn", token::k_return);
      case 's':
        return check_keyword("uper", token::k_super);
      case 't':
        if (std::distance(start_, source_.cend()) > 1) {
          switch (*(start_ + 1)) {
            case 'h':
              return check_keyword("is", token::k_this);
            case 'r':
              return check_keyword("ue", token::k_true);
          }
        }
        break;
      case 'v':
        return check_keyword("ar", token::k_var);
      case 'w':
        return check_keyword("hile", token::k_while);
    }
    return token::identifier;
  }

  std::string source_;
  std::string::const_iterator start_;
  std::string::const_iterator current_;
  int line_;
};

}  // namespace lox

#endif

#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include <string>
#include <vector>

#include "contract.h"
#include "exception.h"

namespace lox {

struct Token {
  enum Type {
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

  Token(Type type, int line) noexcept : type{type}, line{line} {}
  Token(Type type, std::string lexeme, int line) noexcept
      : type{type}, lexeme{std::move(lexeme)}, line{line} {}

  Type type;
  std::string lexeme;
  int line;
};

using Token_vector = std::vector<Token>;

inline bool is_alpha(char ch) noexcept {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

inline bool is_digit(char ch) noexcept { return ch >= '0' && ch <= '9'; }

struct Scanner {
  explicit Scanner(std::string source) noexcept : source{std::move(source)} {}

  Token_vector scan() noexcept {
    current = source.cbegin();
    line = 1;
    Token_vector tokens;
    while (true) {
      tokens.emplace_back(scan_token());
      if (tokens.back().type == Token::eof) {
        break;
      }
    }
    return tokens;
  }

 private:
  Token scan_token() {
    skip_white_space();
    start = current;
    if (is_at_end()) {
      return make_token(Token::eof);
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
        return make_token(Token::left_paren);
      case ')':
        return make_token(Token::right_paren);
      case '{':
        return make_token(Token::left_brace);
      case '}':
        return make_token(Token::right_brace);
      case ';':
        return make_token(Token::semicolon);
      case ',':
        return make_token(Token::comma);
      case '.':
        return make_token(Token::dot);
      case '-':
        return make_token(Token::minus);
      case '+':
        return make_token(Token::plus);
      case '/':
        return make_token(Token::slash);
      case '*':
        return make_token(Token::star);
      case '!':
        return make_token(match('=') ? Token::bang_equal : Token::bang);
      case '=':
        return make_token(match('=') ? Token::equal_equal : Token::equal);
      case '<':
        return make_token(match('=') ? Token::less_equal : Token::less);
      case '>':
        return make_token(match('=') ? Token::greater_equal : Token::greater);
      case '"':
        return string();
    }
    throw make_compiler_error("Unexpected character.");
  }

  Token identifier() noexcept {
    while (!is_at_end() && (is_alpha(peek()) || is_digit(peek()))) {
      advance();
    }
    return make_token(identifier_type());
  }

  Token number() noexcept {
    while (!is_at_end() && is_digit(peek())) {
      advance();
    }
    if (!is_at_end() && peek() == '.' && is_digit(peek_next())) {
      advance();
      while (!is_at_end() && is_digit(peek())) {
        advance();
      }
    }
    return make_token(Token::number);
  }

  Token string() {
    while (!is_at_end() && peek() != '"') {
      if (peek() == '\n') {
        ++line;
      }
      advance();
    }
    if (!is_at_end()) {
      advance();
      return make_token(Token::string);
    }
    throw make_compiler_error("Unterminated string.");
  }

  Token make_token(Token::Type type) const noexcept {
    if (type == Token::string) {
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

  Token::Type check_keyword(int index, const std::string& rest,
                            Token::Type type) const noexcept {
    auto first = start + index;
    auto last = start + index + rest.size();
    if (current == last &&
        std::equal(first, last, rest.cbegin(), rest.cend())) {
      return type;
    }
    return Token::identifier;
  }

  Token::Type identifier_type() const noexcept {
    switch (*start) {
      case 'a':
        return check_keyword(1, "nd", Token::k_and);
      case 'c':
        return check_keyword(1, "lass", Token::k_class);
      case 'e':
        return check_keyword(1, "lse", Token::k_else);
      case 'f':
        if (std::distance(start, source.cend()) > 1) {
          switch (*(start + 1)) {
            case 'a':
              return check_keyword(2, "lse", Token::k_false);
            case 'o':
              return check_keyword(2, "r", Token::k_for);
            case 'u':
              return check_keyword(2, "n", Token::k_func);
          }
        }
        break;
      case 'i':
        return check_keyword(1, "f", Token::k_if);
      case 'n':
        return check_keyword(1, "il", Token::k_nil);
      case 'o':
        return check_keyword(1, "r", Token::k_or);
      case 'p':
        return check_keyword(1, "rint", Token::k_print);
      case 'r':
        return check_keyword(1, "eturn", Token::k_return);
      case 's':
        return check_keyword(1, "uper", Token::k_super);
      case 't':
        if (std::distance(start, source.cend()) > 1) {
          switch (*(start + 1)) {
            case 'h':
              return check_keyword(2, "is", Token::k_this);
            case 'r':
              return check_keyword(2, "ue", Token::k_true);
          }
        }
        break;
      case 'v':
        return check_keyword(1, "ar", Token::k_var);
      case 'w':
        return check_keyword(1, "hile", Token::k_while);
    }
    return Token::identifier;
  }

  Compile_error make_compiler_error(const std::string& message) const {
    std::string string = {start, current};
    return Compile_error{"[line " + std::to_string(line) + "] " +
                         (!string.empty() ? "at '" + string + "' " : "") +
                         message};
  }

  std::string source;
  std::string::const_iterator start;
  std::string::const_iterator current;
  int line;
};

}  // namespace lox

#endif

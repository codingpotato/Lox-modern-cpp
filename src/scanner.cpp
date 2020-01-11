#include "scanner.h"

#include <map>
#include <string>

namespace lox {

token_vector scanner::scan() {
  token_vector tokens;
  current_ = source_.cbegin();
  while (!is_end()) {
    token_start_ = current_;
    scan_token(tokens);
  }
  tokens.emplace_back(token::eof, "", line_);
  return tokens;
}

void scanner::scan_token(token_vector &tokens) {
  auto c = *current_;
  ++current_;
  switch (c) {
  case '(':
    tokens.emplace_back(token::left_paren);
    break;
  case ')':
    tokens.emplace_back(token::right_paren);
    break;
  case '{':
    tokens.emplace_back(token::left_brace);
    break;
  case '}':
    tokens.emplace_back(token::right_brace);
    break;
  case ',':
    tokens.emplace_back(token::comma);
    break;
  case '.':
    tokens.emplace_back(token::dot);
    break;
  case '-':
    tokens.emplace_back(token::minus);
    break;
  case '+':
    tokens.emplace_back(token::plus);
    break;
  case ';':
    tokens.emplace_back(token::semicolon);
    break;
  case '*':
    tokens.emplace_back(token::star);
    break;
  case '!':
    tokens.emplace_back(match('=') ? token::bang_equal : token::bang);
    break;
  case '=':
    tokens.emplace_back(match('=') ? token::equal_equal : token::equal);
    break;
  case '<':
    tokens.emplace_back(match('=') ? token::less_equal : token::less);
    break;
  case '>':
    tokens.emplace_back(match('=') ? token::greater_equal : token::greater);
    break;
  case '/':
    if (match('/')) {
      while (peek() != '\n' && !is_end()) {
        ++current_;
      }
    } else {
      tokens.emplace_back(token::slash);
    }
    break;
  case ' ':
  case '\r':
  case '\t':
    break;
  case '\n':
    line_++;
    break;
  case '"':
    string(tokens);
    break;
  default:
    if (isdigit(c)) {
      number(tokens);
    } else if (isalpha(c)) {
      identifier(tokens);
    } else {
      // throw "Unexpected character."
    }
    break;
  }
}

void scanner::number(token_vector &tokens) {
  while (isdigit(peek())) {
    ++current_;
  }
  if (peek() == '.' && isdigit(peek_next())) {
    ++current_;
    while (isdigit(peek())) {
      ++current_;
    }
    tokens.emplace_back(token::number,
                        std::stod(std::string{token_start_, current_}));
  } else {
    tokens.emplace_back(token::number,
                        std::stoi(std::string{token_start_, current_}));
  }
}

void scanner::identifier(token_vector &tokens) {
  static const std::map<std::string, token::type_t> keywords = {
      {"and", token::k_and},     {"class", token::k_class},
      {"else", token::k_else},   {"false", token::k_false},
      {"for", token::k_for},     {"fun", token::k_fun},
      {"if", token::k_if},       {"nil", token::k_nil},
      {"or", token::k_or},       {"return", token::k_return},
      {"super", token::k_super}, {"this", token::k_this},
      {"true", token::k_true},   {"var", token::k_var},
      {"while", token::k_while}};
  while (is_alpha_numeric(peek())) {
    ++current_;
  }
  auto text = std::string(token_start_, current_);
  if (keywords.find(text) != keywords.cend()) {
    tokens.emplace_back(keywords.at(text));
  } else {
    tokens.emplace_back(token::identifier);
  }
}

void scanner::string(token_vector &tokens) {
  while (peek() != '"' && !is_end()) {
    if (peek() == '\n') {
      line_++;
    }
    ++current_;
  }
  if (is_end()) {
    // throw "Unterminated string"
    return;
  }
  ++current_;
  tokens.emplace_back(token::string,
                      std::string{token_start_ + 1, current_ - 2});
}

} // namespace lox
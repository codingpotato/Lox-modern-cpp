#include "scanner.h"

#include <map>

#include "exception.h"

namespace lox {

token_vector scanner::scan() {
  token_vector tokens;
  current = source.cbegin();
  while (!is_end()) {
    token_start = current;
    scan_token(tokens);
  }
  tokens.emplace_back(token::eof, string{""}, line);
  return tokens;
}

void scanner::scan_token(token_vector &tokens) {
  auto c = *current;
  ++current;
  switch (c) {
    case '(':
      tokens.emplace_back(token::left_paren, line);
      break;
    case ')':
      tokens.emplace_back(token::right_paren, line);
      break;
    case '{':
      tokens.emplace_back(token::left_brace, line);
      break;
    case '}':
      tokens.emplace_back(token::right_brace, line);
      break;
    case ',':
      tokens.emplace_back(token::comma, line);
      break;
    case '.':
      tokens.emplace_back(token::dot, line);
      break;
    case '-':
      tokens.emplace_back(token::minus, line);
      break;
    case '+':
      tokens.emplace_back(token::plus, line);
      break;
    case ';':
      tokens.emplace_back(token::semicolon, line);
      break;
    case '*':
      tokens.emplace_back(token::star, line);
      break;
    case '!':
      tokens.emplace_back(match('=') ? token::bang_equal : token::bang, line);
      break;
    case '=':
      tokens.emplace_back(match('=') ? token::equal_equal : token::equal, line);
      break;
    case '<':
      tokens.emplace_back(match('=') ? token::less_equal : token::less, line);
      break;
    case '>':
      tokens.emplace_back(match('=') ? token::greater_equal : token::greater,
                          line);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !is_end()) {
          ++current;
        }
      } else {
        tokens.emplace_back(token::slash, line);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      ++line;
      break;
    case '"':
      scan_string(tokens);
      break;
    default:
      if (isdigit(c)) {
        scan_number(tokens);
      } else if (isalpha(c)) {
        scan_identifier(tokens);
      } else {
        throw scan_error{"Unexpected character."};
      }
      break;
  }
}

void scanner::scan_number(token_vector &tokens) {
  while (isdigit(peek())) {
    ++current;
  }
  if (peek() == '.' && isdigit(peek_next())) {
    ++current;
    while (isdigit(peek())) {
      ++current;
    }
    tokens.emplace_back(token::l_number,
                        std::stod(string{token_start, current}), line);
  } else {
    tokens.emplace_back(token::l_number,
                        std::stoi(string{token_start, current}), line);
  }
}

void scanner::scan_identifier(token_vector &tokens) {
  static const std::map<string, token::type_t> keywords = {
      {"and", token::k_and},       {"class", token::k_class},
      {"else", token::k_else},     {"false", token::k_false},
      {"for", token::k_for},       {"fun", token::k_fun},
      {"if", token::k_if},         {"nil", token::k_nil},
      {"or", token::k_or},         {"print", token::k_print},
      {"return", token::k_return}, {"super", token::k_super},
      {"this", token::k_this},     {"true", token::k_true},
      {"var", token::k_var},       {"while", token::k_while}};

  while (is_alpha_numeric(peek())) {
    ++current;
  }
  auto text = string(token_start, current);
  if (keywords.find(text) != keywords.cend()) {
    tokens.emplace_back(keywords.at(text), line);
  } else {
    tokens.emplace_back(token::l_identifier, string{token_start, current},
                        line);
  }
}

void scanner::scan_string(token_vector &tokens) {
  while (peek() != '"' && !is_end()) {
    if (peek() == '\n') {
      line++;
    }
    ++current;
  }
  if (is_end()) {
    throw scan_error{"Unterminated string."};
  }
  ++current;
  tokens.emplace_back(token::l_string, string{token_start + 1, current - 2},
                      line);
}

}  // namespace lox

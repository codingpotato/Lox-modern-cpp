#include "scanner.h"

/*namespace lox {

const std::map<std::string, token::type_t> scanner::keywords_ = {
    {"and", token::k_and},     {"class", token::k_class},
    {"else", token::k_else},   {"false", token::k_false},
    {"for", token::k_for},     {"fun", token::k_fun},
    {"if", token::k_if},       {"nil", token::k_nil},
    {"or", token::k_or},       {"return", token::k_return},
    {"super", token::k_super}, {"this", token::k_this},
    {"true", token::k_true},   {"var", token::k_var},
    {"while", token::k_while}};

std::vector<token> scanner::scan_tokens() {
  while (!is_end()) {
    start_ = current_;
    scan_token();
  }
  tokens_.emplace_back(token::eof, "", line_);
  return std::move(tokens_);
}

void scanner::scan_token() {
  auto c = advance();
  switch (c) {
    case '(':
      add_token(token::left_paren);
      break;
    case ')':
      add_token(token::right_paren);
      break;
    case '{':
      add_token(token::left_brace);
      break;
    case '}':
      add_token(token::right_brace);
      break;
    case ',':
      add_token(token::comma);
      break;
    case '.':
      add_token(token::dot);
      break;
    case '-':
      add_token(token::minus);
      break;
    case '+':
      add_token(token::plus);
      break;
    case ';':
      add_token(token::semicolon);
      break;
    case '*':
      add_token(token::star);
      break;
    case '!':
      add_token(match('=') ? token::bang_equal : token::bang);
      break;
    case '=':
      add_token(match('=') ? token::equal_equal : token::equal);
      break;
    case '<':
      add_token(match('=') ? token::less_equal : token::less);
      break;
    case '>':
      add_token(match('=') ? token::greater_equal : token::greater);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !is_end()) advance();
      } else {
        add_token(token::slash);
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
      string();
      break;
    default:
      if (isdigit(c)) {
        number();
      } else if (isalpha(c)) {
        identifier();
      } else {
        // throw "Unexpected character."
      }
      break;
  }
}

void scanner::number() {
  while (isdigit(peek())) advance();
  if (peek() == '.' && isdigit(peek_next())) {
    advance();
    while (isdigit(peek())) advance();
    add_token(token::number,
              std::stod(source_.substr(start_, current_ - start_)));
  } else {
    add_token(token::number,
              std::stoi(source_.substr(start_, current_ - start_)));
  }
}

void scanner::identifier() {
  while (isalphanumeric(peek())) advance();
  auto text = source_.substr(start_, current_ - start_);
  if (keywords_.find(text) != keywords_.end()) {
    add_token(keywords_.at(text));
  } else {
    add_token(token::identifier);
  }
}

void scanner::string() {
  while (peek() != '"' && !is_end()) {
    if (peek() == '\n') line_++;
    advance();
  }
  if (is_end()) {
    // throw "Unterminated string"
    return;
  }
  advance();
  add_token(token::string, source_.substr(start_ + 1, current_ - start_ - 2));
}

}  // namespace lox */
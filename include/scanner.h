#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "token.h"

namespace lox {

class scanner {
 public:
  explicit scanner(std::string src) noexcept : source{std::move(src)} {}

  token_vector scan();

 private:
  void scan_token(token_vector &tokens);

  void number(token_vector &tokens);
  void identifier(token_vector &tokens);
  void string(token_vector &tokens);

  char peek() const noexcept {
    if (is_end()) {
      return '\0';
    }
    return *current;
  }

  char peek_next() const noexcept {
    if (current == source.cend()) {
      return '\0';
    }
    return *(current + 1);
  }

  bool is_end() const noexcept { return current == source.cend(); }

  bool is_alpha(char ch) noexcept { return isalpha(ch) || ch == '_'; }
  bool is_alpha_numeric(char ch) noexcept { return isalpha(ch) || isdigit(ch); }

  bool match(char expect) noexcept {
    if (is_end() || *current != expect) {
      return false;
    }
    ++current;
    return true;
  }

  std::string source;
  std::string::const_iterator token_start;
  std::string::const_iterator current;
  int line = 1;
};

}  // namespace lox

#endif
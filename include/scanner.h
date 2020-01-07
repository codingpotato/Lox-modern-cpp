#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "token.h"
namespace lox {

struct scanner {
  scanner(std::string source) noexcept : source_{std::move(source)} {}

  token_vector scan();

private:
  void scan_token();

  void number();
  void identifier();
  void string();

  char peek() const noexcept {
    if (is_end()) {
      return '\0';
    }
    return *current_;
  }

  char peek_next() const noexcept {
    if (current_ == source_.cend()) {
      return '\0';
    }
    return *(current_ + 1);
  }

  bool is_end() const noexcept { return current_ == source_.cend(); }

  bool is_alpha(char ch) noexcept { return isalpha(ch) || ch == '_'; }
  bool is_alpha_numeric(char ch) noexcept { return isalpha(ch) || isdigit(ch); }

  bool match(char expect) noexcept {
    if (is_end() || *current_ != expect) {
      return false;
    }
    current_++;
    return true;
  }

  std::string source_;
  std::string::const_iterator token_start_;
  std::string::const_iterator current_;
  int line_ = 1;
  token_vector tokens_;
};

} // namespace lox

#endif
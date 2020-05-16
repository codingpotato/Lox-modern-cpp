#include "compiler.h"

namespace lox {

Compile_error Compiler::make_compile_error(const std::string& message,
                                           const Token& token) noexcept {
  return Compile_error{
      "[line " + std::to_string(token.line) + "] Error at " +
      (token.type != Token::eof ? "'" + token.lexeme + "': " : "end") +
      message};
}

Compile_error Compiler::make_compile_error(const std::string& message,
                                           bool from_current) const noexcept {
  auto it = from_current ? current : previous;
  ENSURES(it != tokens.cend());
  return make_compile_error(message, *it);
}

}  // namespace lox

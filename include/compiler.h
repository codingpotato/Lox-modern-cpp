#ifndef LOX_COMPILER_H
#define LOX_COMPILER_H

#include <string>

#include "exception.h"
#include "chunk.h"
#include "scanner.h"

namespace lox {

struct compiler {
  chunk compile(token_vector tokens) noexcept {
    tokens_ = std::move(tokens);
    current_ = tokens_.cbegin();
    chunk ch;
    advance();
    expression(ch);
    consume(token::eof, "Expect end of expression.");
    return ch;
  }

 private:
  void expression(chunk&) noexcept {}

  void number(chunk& ch) const noexcept {
    auto constant = ch.add_constant(std::stod(previous_->lexeme));
    ch.add_instruction(instruction::op_constant, constant, previous_->line);
  }

  void grouping(chunk& ch) noexcept {
    expression(ch);
    consume(token::right_paren, "Expect ')' after expression.");
  }

  void unary(chunk& ch) {
    auto op_type = previous_->type;
    expression(ch);
    switch (op_type) {
      case token::minus:
        ch.add_instruction(instruction::op_negate, previous_->line);
        break;
      default:
        return;
    }
  }

  void advance() noexcept {
    previous_ = current_;
    ++current_;
  }

  void consume(token::type_t type, const std::string& message) {
    if (current_->type == type) {
      advance();
    } else {
      throw compile_error{message};
    }
  }

  token_vector tokens_;
  token_vector::const_iterator current_;
  token_vector::const_iterator previous_;
};

}  // namespace lox

#endif

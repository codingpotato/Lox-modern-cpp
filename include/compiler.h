#ifndef LOX_COMPILER_H
#define LOX_COMPILER_H

#include <array>
#include <string>

#include "chunk.h"
#include "exception.h"
#include "scanner.h"

namespace lox {

struct compiler {
  chunk compile(token_vector tokens) noexcept {
    tokens_ = std::move(tokens);
    current_ = tokens_.cbegin();
    chunk ch;
    expression(ch);
    consume(token::eof, "Expect end of expression.");
    return ch;
  }

 private:
  enum precedence {
    p_none,
    p_assignment,  // =
    p_or,          // or
    p_and,         // and
    p_equality,    // == !=
    p_comparison,  // < > <= >=
    p_term,        // + -
    p_factor,      // * /
    p_unary,       // ! -
    p_call,        // . ()
    p_primary
  };

  using parse_func = void (compiler::*)(chunk&);

  struct rule {
    parse_func prefix;
    parse_func infix;
    precedence prec;
  };

  void expression(chunk& ch) noexcept { parse_precedence(p_assignment, ch); }

  void number(chunk& ch) {
    auto constant = ch.add_constant(std::stod(previous_->lexeme));
    ch.add_instruction(op_constant{}, constant, previous_->line);
  }

  void literal(chunk& ch) {
    switch (previous_->type) {
      case token::k_nil:
        ch.add_instruction(op_nil{}, previous_->line);
        break;
      case token::k_false:
        ch.add_instruction(op_false{}, previous_->line);
        break;
      case token::k_true:
        ch.add_instruction(op_true{}, previous_->line);
        break;
      default:
        throw internal_error{"Unknow literal."};
    }
  }

  void binary(chunk& ch) {
    const auto op_type = previous_->type;
    parse_precedence(static_cast<precedence>(rules[op_type].prec + 1), ch);
    switch (op_type) {
      case token::plus:
        ch.add_instruction(op_add{}, previous_->line);
        break;
      case token::minus:
        ch.add_instruction(op_subtract{}, previous_->line);
        break;
      case token::star:
        ch.add_instruction(op_multiply{}, previous_->line);
        break;
      case token::slash:
        ch.add_instruction(op_divide{}, previous_->line);
        break;
      default:
        break;
    }
  }

  void grouping(chunk& ch) {
    expression(ch);
    consume(token::right_paren, "Expect ')' after expression.");
  }

  void unary(chunk& ch) {
    auto op_type = previous_->type;
    parse_precedence(p_unary, ch);
    switch (op_type) {
      case token::bang:
        ch.add_instruction(op_not{}, previous_->line);
        break;
      case token::minus:
        ch.add_instruction(op_negate{}, previous_->line);
        break;
      default:
        break;
    }
  }

  void parse_precedence(precedence prec, chunk& ch) {
    advance();
    const auto& prefix = rules[previous_->type].prefix;
    if (prefix == nullptr) {
      throw compile_error{"Expect expression."};
      return;
    }
    (this->*prefix)(ch);
    while (static_cast<int>(prec) <
           static_cast<int>(rules[current_->type].prec)) {
      advance();
      auto infix = rules[previous_->type].infix;
      (this->*infix)(ch);
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

  constexpr static rule rules[] = {
      {&compiler::grouping, nullptr, p_none},         // token::left_paren
      {nullptr, nullptr, p_none},                     // token::right_paren
      {nullptr, nullptr, p_none},                     // token::left_brace
      {nullptr, nullptr, p_none},                     // token::right_brace
      {nullptr, nullptr, p_none},                     // token::comma
      {nullptr, nullptr, p_none},                     // token::dot
      {&compiler::unary, &compiler::binary, p_term},  // token::minus
      {nullptr, &compiler::binary, p_term},           // token::plus
      {nullptr, nullptr, p_none},                     // token::demicolon
      {nullptr, &compiler::binary, p_factor},         // token::slash
      {nullptr, &compiler::binary, p_factor},         // token::star
      {&compiler::unary, nullptr, p_none},            // token::bang
      {nullptr, nullptr, p_none},                     // token::bang_equal
      {nullptr, nullptr, p_none},                     // token::equal
      {nullptr, nullptr, p_none},                     // token::equal_equal
      {nullptr, nullptr, p_none},                     // token::greater
      {nullptr, nullptr, p_none},                     // token::greater_equal
      {nullptr, nullptr, p_none},                     // token::less
      {nullptr, nullptr, p_none},                     // token::less_equal
      {nullptr, nullptr, p_none},                     // token::identifier
      {&compiler::number, nullptr, p_none},           // token::number
      {nullptr, nullptr, p_none},                     // token::string
      {nullptr, nullptr, p_none},                     // token::k_and
      {nullptr, nullptr, p_none},                     // token::k_class
      {nullptr, nullptr, p_none},                     // token::k_else
      {&compiler::literal, nullptr, p_none},          // token::k_false
      {nullptr, nullptr, p_none},                     // token::k_for
      {nullptr, nullptr, p_none},                     // token::k_func
      {nullptr, nullptr, p_none},                     // token::k_if
      {&compiler::literal, nullptr, p_none},          // token::k_nil
      {nullptr, nullptr, p_none},                     // token::k_or
      {nullptr, nullptr, p_none},                     // token::k_print
      {nullptr, nullptr, p_none},                     // token::k_return
      {nullptr, nullptr, p_none},                     // token::k_super
      {nullptr, nullptr, p_none},                     // token::k_this
      {&compiler::literal, nullptr, p_none},          // token::k_true
      {nullptr, nullptr, p_none},                     // token::k_var
      {nullptr, nullptr, p_none},                     // token::k_while
      {nullptr, nullptr, p_none},                     // token::eof
  };

  token_vector tokens_;
  token_vector::const_iterator current_;
  token_vector::const_iterator previous_;
};

}  // namespace lox

#endif

#ifndef LOX_COMPILER_H
#define LOX_COMPILER_H

#include <array>
#include <string>

#include "chunk.h"
#include "exception.h"
#include "scanner.h"

namespace lox {

namespace {

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

template <typename Compiler>
using parse_func = void (Compiler::*)(chunk&);

template <typename Compiler>
struct precedence_rule {
  parse_func<Compiler> prefix;
  parse_func<Compiler> infix;
  precedence prec;
};

template <typename Compiler>
using precedence_rules = std::array<precedence_rule<Compiler>,
                                    static_cast<std::size_t>(token::eof) + 1>;

template <typename Compiler>
struct rules_generator {
  static constexpr precedence_rules<Compiler> make_rules() noexcept {
    struct element {
      token::type_t type;
      precedence_rule<Compiler> rule;
    };
    constexpr element elements[] = {
        {token::left_paren, {&Compiler::grouping, nullptr, p_none}},
        {token::minus, {&Compiler::unary, &Compiler::binary, p_term}},
        {token::plus, {nullptr, &Compiler::binary, p_term}},
        {token::slash, {nullptr, &Compiler::binary, p_factor}},
        {token::star, {nullptr, &Compiler::binary, p_factor}},
        {token::bang, {&Compiler::unary, nullptr, p_none}},
        {token::bang_equal, {nullptr, &Compiler::binary, p_equality}},
        {token::equal, {nullptr, &Compiler::binary, p_comparison}},
        {token::equal_equal, {nullptr, &Compiler::binary, p_comparison}},
        {token::greater, {nullptr, &Compiler::binary, p_comparison}},
        {token::greater_equal, {nullptr, &Compiler::binary, p_comparison}},
        {token::less, {nullptr, &Compiler::binary, p_comparison}},
        {token::less_equal, {nullptr, &Compiler::binary, p_comparison}},
        {token::number, {&Compiler::number, nullptr, p_none}},
        {token::string, {&Compiler::string, nullptr, p_none}},
        {token::k_false, {&Compiler::literal, nullptr, p_none}},
        {token::k_nil, {&Compiler::literal, nullptr, p_none}},
        {token::k_true, {&Compiler::literal, nullptr, p_none}},
    };
    precedence_rules<Compiler> rules{};
    for (auto it = std::cbegin(elements); it != std::end(elements); ++it) {
      rules[static_cast<std::size_t>(it->type)] = it->rule;
    }
    return rules;
  };
};

}  // namespace

class compiler {
 public:
  chunk compile(token_vector tokens) noexcept {
    tokens_ = std::move(tokens);
    current_ = tokens_.cbegin();
    chunk ch;
    while (!match(token::eof)) {
      declaration(ch);
    }
    return ch;
  }

 private:
  void declaration(chunk& ch) {
    if (match(token::k_var)) {
      var_declaration(ch);
    } else {
      statement(ch);
    }
  }

  void var_declaration(chunk& ch) {
    auto global = parse_variable(ch, "Expect variable name.");
    if (match(token::equal)) {
      expression(ch);
    } else {
      ch.add_instruction(op_nil{}, previous_->line);
    }
    consume(token::semicolon, "Expect ';' after variable declaration.");
    define_variable(ch, global);
  }

  std::size_t parse_variable(chunk& ch, const std::string& message) {
    consume(token::identifier, message);
    return ch.add_constant(previous_->lexeme);
  }

  void define_variable(chunk& ch, std::size_t global) {
    ch.add_instruction(op_define_global{}, global, previous_->line);
  }

  void statement(chunk& ch) {
    if (match(token::k_print)) {
      print_statement(ch);
    } else {
      expression_statement(ch);
    }
  }

  void print_statement(chunk& ch) {
    expression(ch);
    consume(token::semicolon, "Expect ';' after value.");
    ch.add_instruction(op_print{}, previous_->line);
  }

  void expression_statement(chunk& ch) {
    expression(ch);
    consume(token::semicolon, "Expect ';' after value.");
    ch.add_instruction(op_pop{}, previous_->line);
  }

  void expression(chunk& ch) { parse_precedence(p_assignment, ch); }

  void binary(chunk& ch) {
    const auto op_type = previous_->type;
    parse_precedence(static_cast<precedence>(p_rules_[op_type].prec + 1), ch);
    switch (op_type) {
      case token::bang_equal:
        ch.add_instruction(op_equal{}, previous_->line);
        ch.add_instruction(op_not{}, previous_->line);
        break;
      case token::equal_equal:
        ch.add_instruction(op_equal{}, previous_->line);
        break;
      case token::greater:
        ch.add_instruction(op_greater{}, previous_->line);
        break;
      case token::greater_equal:
        ch.add_instruction(op_less{}, previous_->line);
        ch.add_instruction(op_not{}, previous_->line);
        break;
      case token::less:
        ch.add_instruction(op_less{}, previous_->line);
        break;
      case token::less_equal:
        ch.add_instruction(op_greater{}, previous_->line);
        ch.add_instruction(op_not{}, previous_->line);
        break;
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

  void number(chunk& ch) {
    auto constant = ch.add_constant(std::stod(previous_->lexeme));
    ch.add_instruction(op_constant{}, constant, previous_->line);
  }

  void string(chunk& ch) {
    auto constant = ch.add_constant(previous_->lexeme);
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

  void parse_precedence(precedence prec, chunk& ch) {
    advance();
    const auto& prefix = p_rules_[previous_->type].prefix;
    if (prefix == nullptr) {
      throw compile_error{"Expect expression."};
      return;
    }
    (this->*prefix)(ch);
    while (static_cast<int>(prec) <
           static_cast<int>(p_rules_[current_->type].prec)) {
      advance();
      auto infix = p_rules_[previous_->type].infix;
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

  bool match(token::type_t type) noexcept {
    if (current_->type == type) {
      advance();
      return true;
    }
    return false;
  }

  friend rules_generator<compiler>;
  static constexpr precedence_rules<compiler> p_rules_ =
      rules_generator<compiler>::make_rules();

  token_vector tokens_;
  token_vector::const_iterator current_;
  token_vector::const_iterator previous_;
};

}  // namespace lox

#endif

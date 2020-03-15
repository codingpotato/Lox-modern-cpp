#ifndef LOX_COMPILER_H
#define LOX_COMPILER_H

#include <array>
#include <string>

#include "chunk.h"
#include "exception.h"
#include "scanner.h"

namespace lox {

namespace precedence {

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
using parse_func = void (Compiler::*)(chunk&, bool can_assign);

template <typename Compiler>
struct rule {
  parse_func<Compiler> prefix;
  parse_func<Compiler> infix;
  precedence prec;
};

template <typename Compiler>
using rules =
    std::array<rule<Compiler>, static_cast<std::size_t>(token::eof) + 1>;

template <typename Compiler>
struct rules_generator {
  static constexpr rules<Compiler> make_rules() noexcept {
    struct element {
      token::type_t type;
      rule<Compiler> rule_of_type;
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
        {token::identifier, {&Compiler::add_variable, nullptr, p_none}},
        {token::number, {&Compiler::add_number, nullptr, p_none}},
        {token::string, {&Compiler::add_string, nullptr, p_none}},
        {token::k_false, {&Compiler::add_literal, nullptr, p_none}},
        {token::k_nil, {&Compiler::add_literal, nullptr, p_none}},
        {token::k_true, {&Compiler::add_literal, nullptr, p_none}},
    };
    rules<Compiler> rules{};
    for (auto it = std::cbegin(elements); it != std::end(elements); ++it) {
      rules[static_cast<std::size_t>(it->type)] = it->rule_of_type;
    }
    return rules;
  };
};

}  // namespace precedence

class compiler {
 public:
  chunk compile(token_vector tokens) noexcept {
    tokens_ = std::move(tokens);
    current_ = tokens_.cbegin();
    scope_depth_ = 0;
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
    const auto name = variable_name_constant(ch, "Expect variable name.");
    if (match(token::equal)) {
      expression(ch);
    } else {
      ch.add_instruction(op_nil{}, previous_->line);
    }
    consume(token::semicolon, "Expect ';' after variable declaration.");
    define_variable(ch, name);
  }

  std::size_t variable_name_constant(chunk& ch, const std::string& message) {
    consume(token::identifier, message);

    declare_variable();
    if (scope_depth_ > 0) {
      return 0;
    }
    return ch.add_constant(previous_->lexeme);
  }

  void declare_variable() {
    if (scope_depth_ > 0) {
      const auto& name = previous_->lexeme;
      for (auto it = locals_.crbegin(); it != locals_.crend(); ++it) {
        if (it->depth != -1 && it->depth < scope_depth_) {
          break;
        }
        if (it->name == name) {
          throw compile_error{"Variable '" + name +
                              "' already declared in this scope."};
        }
      }
      locals_.emplace_back(previous_->lexeme, -1);
    }
  }

  void define_variable(chunk& ch, std::size_t name) noexcept {
    if (scope_depth_ > 0) {
      ENSURES(!locals_.empty());
      locals_.back().depth = scope_depth_;
    } else {
      ch.add_instruction(op_define_global{}, name, previous_->line);
    }
  }

  void statement(chunk& ch) {
    if (match(token::k_print)) {
      print_statement(ch);
    } else if (match(token::k_if)) {
      parse_if(ch);
    } else if (match(token::left_brace)) {
      begin_scope();
      parse_block(ch);
      end_scope(ch);
    } else {
      expression_statement(ch);
    }
  }

  void parse_if(chunk& ch) {
    consume(token::left_paren, "Expect '(' after 'if'.");
    expression(ch);
    consume(token::right_paren, "Expect ')' after condition.");

    const auto then_jump_index =
        ch.add_instruction(op_jump_if_false{}, 0, previous_->line);
    ch.add_instruction(op_pop{}, previous_->line);
    statement(ch);
    const auto else_jump_index =
        ch.add_instruction(op_jump{}, 0, previous_->line);

    ch.set_oprand(then_jump_index, ch.code().size() - (then_jump_index + 1));
    ch.add_instruction(op_pop{}, previous_->line);
    if (match(token::k_else)) {
      statement(ch);
    }
    ch.set_oprand(else_jump_index, ch.code().size() - (else_jump_index + 1));
  }

  void parse_block(chunk& ch) {
    while (!check(token::right_brace) && !check(token::eof)) {
      declaration(ch);
    }
    consume(token::right_brace, "Expect '}' after block.");
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

  void expression(chunk& ch) { parse_precedence(precedence::p_assignment, ch); }

  void binary(chunk& ch, bool) {
    const auto op_type = previous_->type;
    parse_precedence(
        static_cast<precedence::precedence>(p_rules_[op_type].prec + 1), ch);
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

  void grouping(chunk& ch, bool) {
    expression(ch);
    consume(token::right_paren, "Expect ')' after expression.");
  }

  void unary(chunk& ch, bool) {
    auto op_type = previous_->type;
    parse_precedence(precedence::p_unary, ch);
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

  int resolve_local(const std::string& name) {
    for (int i = locals_.size() - 1; i >= 0; --i) {
      if (locals_[i].name == name) {
        if (locals_[i].depth != -1) {
          return i;
        }
        throw compile_error{
            "Cannot read local variable in its own initializer."};
      }
    }
    return -1;
  }

  void add_variable(chunk& ch, bool can_assign) {
    auto index = resolve_local(previous_->lexeme);
    bool is_global = false;
    if (index == -1) {
      is_global = true;
      index = ch.add_constant(previous_->lexeme);
    }
    if (can_assign && match(token::equal)) {
      expression(ch);
      if (is_global) {
        ch.add_instruction(op_set_global{}, index, previous_->line);
      } else {
        ch.add_instruction(op_set_local{}, index, previous_->line);
      }
    } else {
      if (is_global) {
        ch.add_instruction(op_get_global{}, index, previous_->line);
      } else {
        ch.add_instruction(op_get_local{}, index, previous_->line);
      }
    }
  }

  void add_number(chunk& ch, bool) {
    const auto constant = ch.add_constant(std::stod(previous_->lexeme));
    ch.add_instruction(op_constant{}, constant, previous_->line);
  }

  void add_string(chunk& ch, bool) {
    const auto constant = ch.add_constant(previous_->lexeme);
    ch.add_instruction(op_constant{}, constant, previous_->line);
  }

  void add_literal(chunk& ch, bool) {
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

  void parse_precedence(precedence::precedence prec, chunk& ch) {
    advance();
    const auto& prefix = p_rules_[previous_->type].prefix;
    if (prefix == nullptr) {
      throw compile_error{"Expect expression."};
      return;
    }
    const auto can_assign = prec <= precedence::p_assignment;
    (this->*prefix)(ch, can_assign);
    while (static_cast<int>(prec) <
           static_cast<int>(p_rules_[current_->type].prec)) {
      advance();
      auto infix = p_rules_[previous_->type].infix;
      (this->*infix)(ch, can_assign);
    }

    if (can_assign && match(token::equal)) {
      throw compile_error{"Invalid assignment target."};
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

  bool check(token::type_t type) noexcept { return current_->type == type; }

  bool match(token::type_t type) noexcept {
    if (check(type)) {
      advance();
      return true;
    }
    return false;
  }

  void begin_scope() noexcept { ++scope_depth_; }
  void end_scope(chunk& ch) noexcept {
    --scope_depth_;
    while (!locals_.empty() && locals_.back().depth > scope_depth_) {
      ch.add_instruction(op_pop{}, previous_->line);
      locals_.pop_back();
    }
  }

  struct local {
    local(std::string n, int d) noexcept : name{std::move(n)}, depth{d} {}

    std::string name;
    int depth;
  };

  friend precedence::rules_generator<compiler>;
  static constexpr precedence::rules<compiler> p_rules_ =
      precedence::rules_generator<compiler>::make_rules();

  token_vector tokens_;
  token_vector::const_iterator current_;
  token_vector::const_iterator previous_;
  std::vector<local> locals_;
  int scope_depth_;
};

}  // namespace lox

#endif

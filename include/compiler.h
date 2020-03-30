#ifndef LOX_COMPILER_H
#define LOX_COMPILER_H

#include <array>
#include <string>

#include "chunk.h"
#include "exception.h"
#include "scanner.h"
#include "virtual_machine.h"

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
using parse_func = void (Compiler::*)(bool can_assign);

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
        {token::k_and, {nullptr, &Compiler::parse_and, p_and}},
        {token::k_or, {nullptr, &Compiler::parse_or, p_or}},
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
  explicit compiler(virtual_machine& vm) noexcept : vm_{vm} {}

  void compile(token_vector tokens) noexcept {
    tokens_ = std::move(tokens);
    current_ = tokens_.cbegin();
    scope_depth_ = 0;
    while (!match(token::eof)) {
      declaration();
    }
  }

 private:
  void declaration() {
    if (match(token::k_var)) {
      var_declaration();
    } else {
      statement();
    }
  }

  void var_declaration() {
    const auto name = variable_name_constant("Expect variable name.");
    if (match(token::equal)) {
      expression();
    } else {
      add_instruction(op_nil{});
    }
    consume(token::semicolon, "Expect ';' after variable declaration.");
    define_variable(name);
  }

  std::size_t variable_name_constant(const std::string& message) {
    consume(token::identifier, message);
    declare_variable();
    if (scope_depth_ > 0) {
      return 0;
    }
    return vm_.add_constant_string(previous_->lexeme);
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

  void define_variable(std::size_t name) noexcept {
    if (scope_depth_ > 0) {
      ENSURES(!locals_.empty());
      locals_.back().depth = scope_depth_;
    } else {
      add_instruction(op_define_global{}, name);
    }
  }

  void statement() {
    if (match(token::k_print)) {
      print_statement();
    } else if (match(token::k_for)) {
      parse_for();
    } else if (match(token::k_if)) {
      parse_if();
    } else if (match(token::k_while)) {
      parse_while();
    } else if (match(token::left_brace)) {
      begin_scope();
      parse_block();
      end_scope();
    } else {
      expression_statement();
    }
  }

  void parse_for() {
    begin_scope();
    consume(token::left_paren, "Expect '(' after 'for'.");
    if (match(token::semicolon)) {
    } else if (match(token::k_var)) {
      var_declaration();
    } else {
      expression_statement();
    }
    auto loop_start = vm_.code_size();
    auto exit_jump = -1;
    if (!match(token::semicolon)) {
      expression();
      consume(token::semicolon, "Expect ';' after loop condition.");
      exit_jump = add_instruction(op_jump_if_false{});
      add_instruction(op_pop{});
    }

    if (!match(token::right_paren)) {
      auto body_jump = add_instruction(op_jump{});
      auto increament_start = vm_.code_size();
      expression();
      add_instruction(op_pop{});
      consume(token::right_paren, "Expect ')' after for clauses.");
      add_instruction(op_loop{}, vm_.code_size() - loop_start + 1);
      loop_start = increament_start;
      patch_jump(body_jump);
    }

    statement();
    add_instruction(op_loop{}, vm_.code_size() - loop_start + 1);
    if (exit_jump != -1) {
      patch_jump(exit_jump);
      add_instruction(op_pop{});
    }
    end_scope();
  }

  void parse_if() {
    consume(token::left_paren, "Expect '(' after 'if'.");
    expression();
    consume(token::right_paren, "Expect ')' after condition.");

    const auto then_jump_index = add_instruction(op_jump_if_false{});
    add_instruction(op_pop{});
    statement();
    const auto else_jump_index = add_instruction(op_jump{});

    patch_jump(then_jump_index);
    add_instruction(op_pop{});
    if (match(token::k_else)) {
      statement();
    }
    patch_jump(else_jump_index);
  }

  void parse_while() {
    const auto loop_start = vm_.code_size();
    consume(token::left_paren, "Expect '(' after 'while'.");
    expression();
    consume(token::right_paren, "Expect ')' after condition.");
    const auto exit_jump_index = add_instruction(op_jump_if_false{}, 0);
    add_instruction(op_pop{});
    statement();
    add_instruction(op_loop{}, vm_.code_size() - loop_start + 1);
    patch_jump(exit_jump_index);
    add_instruction(op_pop{});
  }

  void parse_block() {
    while (!check(token::right_brace) && !check(token::eof)) {
      declaration();
    }
    consume(token::right_brace, "Expect '}' after block.");
  }

  void print_statement() {
    expression();
    consume(token::semicolon, "Expect ';' after value.");
    add_instruction(op_print{});
  }

  void expression_statement() {
    expression();
    consume(token::semicolon, "Expect ';' after value.");
    add_instruction(op_pop{});
  }

  void expression() { parse_precedence(precedence::p_assignment); }

  void binary(bool) {
    const auto op_type = previous_->type;
    parse_precedence(
        static_cast<precedence::precedence>(p_rules_[op_type].prec + 1));
    switch (op_type) {
      case token::bang_equal:
        add_instruction(op_equal{});
        add_instruction(op_not{});
        break;
      case token::equal_equal:
        add_instruction(op_equal{});
        break;
      case token::greater:
        add_instruction(op_greater{});
        break;
      case token::greater_equal:
        add_instruction(op_less{});
        add_instruction(op_not{});
        break;
      case token::less:
        add_instruction(op_less{});
        break;
      case token::less_equal:
        add_instruction(op_greater{});
        add_instruction(op_not{});
        break;
      case token::plus:
        add_instruction(op_add{});
        break;
      case token::minus:
        add_instruction(op_subtract{});
        break;
      case token::star:
        add_instruction(op_multiply{});
        break;
      case token::slash:
        add_instruction(op_divide{});
        break;
      default:
        break;
    }
  }

  void grouping(bool) {
    expression();
    consume(token::right_paren, "Expect ')' after expression.");
  }

  void unary(bool) {
    auto op_type = previous_->type;
    parse_precedence(precedence::p_unary);
    switch (op_type) {
      case token::bang:
        add_instruction(op_not{});
        break;
      case token::minus:
        add_instruction(op_negate{});
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

  void add_variable(bool can_assign) {
    auto index = resolve_local(previous_->lexeme);
    bool is_global = false;
    if (index == -1) {
      is_global = true;
      index = vm_.add_constant_string(previous_->lexeme);
    }
    if (can_assign && match(token::equal)) {
      expression();
      if (is_global) {
        add_instruction(op_set_global{}, index);
      } else {
        add_instruction(op_set_local{}, index);
      }
    } else {
      if (is_global) {
        add_instruction(op_get_global{}, index);
      } else {
        add_instruction(op_get_local{}, index);
      }
    }
  }

  void add_number(bool) {
    const auto constant = vm_.add_constant_number(std::stod(previous_->lexeme));
    add_instruction(op_constant{}, constant);
  }

  void add_string(bool) {
    const auto constant = vm_.add_constant_string(previous_->lexeme);
    add_instruction(op_constant{}, constant);
  }

  void add_literal(bool) {
    switch (previous_->type) {
      case token::k_nil:
        add_instruction(op_nil{});
        break;
      case token::k_false:
        add_instruction(op_false{});
        break;
      case token::k_true:
        add_instruction(op_true{});
        break;
      default:
        throw internal_error{"Unknow literal."};
    }
  }

  void parse_and(bool) {
    const auto end_jump_index = add_instruction(op_jump_if_false{}, 0);
    add_instruction(op_pop{});
    parse_precedence(precedence::p_and);
    patch_jump(end_jump_index);
  }

  void parse_or(bool) {
    const auto else_jump_index = add_instruction(op_jump_if_false{}, 0);
    const auto end_jump_index = add_instruction(op_jump{}, 0);
    patch_jump(else_jump_index);
    add_instruction(op_pop{});
    parse_precedence(precedence::p_or);
    patch_jump(end_jump_index);
  }

  void parse_precedence(precedence::precedence prec) {
    advance();
    const auto& prefix = p_rules_[previous_->type].prefix;
    if (prefix == nullptr) {
      throw compile_error{"Expect expression."};
      return;
    }
    const auto can_assign = prec <= precedence::p_assignment;
    (this->*prefix)(can_assign);
    while (static_cast<int>(prec) <
           static_cast<int>(p_rules_[current_->type].prec)) {
      advance();
      auto infix = p_rules_[previous_->type].infix;
      (this->*infix)(can_assign);
    }

    if (can_assign && match(token::equal)) {
      throw compile_error{"Invalid assignment target."};
    }
  }

  template <typename Opcode>
  size_t add_instruction(Opcode opcode, oprand_t oprand = 0) noexcept {
    return vm_.add_instruction(previous_->line, opcode, oprand);
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
  void end_scope() noexcept {
    --scope_depth_;
    while (!locals_.empty() && locals_.back().depth > scope_depth_) {
      add_instruction(op_pop{});
      locals_.pop_back();
    }
  }

  void patch_jump(size_t jump) noexcept {
    vm_.set_oprand(jump, vm_.code_size() - (jump + 1));
  }

  struct local {
    local(std::string n, int d) noexcept : name{std::move(n)}, depth{d} {}

    std::string name;
    int depth;
  };

  friend precedence::rules_generator<compiler>;
  static constexpr precedence::rules<compiler> p_rules_ =
      precedence::rules_generator<compiler>::make_rules();

  virtual_machine& vm_;
  token_vector tokens_;
  token_vector::const_iterator current_;
  token_vector::const_iterator previous_;
  std::vector<local> locals_;
  int scope_depth_;
};

}  // namespace lox

#endif

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
        {token::left_paren,
         {&Compiler::grouping, &Compiler::parse_call, p_call}},
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
        {token::number, {&Compiler::add_number_constant, nullptr, p_none}},
        {token::string, {&Compiler::add_string_constant, nullptr, p_none}},
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

  function* compile(token_vector tokens) noexcept {
    make_func_frame(nullptr, 0);
    tokens_ = std::move(tokens);
    current_ = tokens_.cbegin();
    current_func_frame().push_local();
    while (!match(token::eof)) {
      parse_declaration();
    }
    add_return_instruction();
    EXPECTS(func_frames.size() == 1)
    return current_func_frame().func;
  }

 private:
  void parse_declaration() {
    if (match(token::k_func)) {
      parse_func_declaration();
    } else if (match(token::k_var)) {
      parse_var_declaration();
    } else {
      parse_statement();
    }
  }

  void parse_func_declaration() {
    const auto name = parse_variable("Expect function name.");
    current_func_frame().latest_local_initialized();
    parse_function();
    current_func_frame().define_variable(name, previous_->line);
  }

  void parse_function() {
    make_func_frame(vm_.main_heap.make_string(previous_->lexeme), 1);
    current_func_frame().begin_scope();
    consume(token::left_paren, "Expect '(' after function name.");
    if (!check(token::right_paren)) {
      do {
        auto parameter = parse_variable("Expect parameter name.");
        current_func_frame().define_variable(parameter, previous_->line);
        ++current_func_frame().func->arity;
        if (current_func_frame().func->arity > max_function_parameters) {
          throw runtime_error{"Cannot have more than " +
                              std::to_string(max_function_parameters) +
                              " parameters."};
        }
      } while (match(token::comma));
    }
    consume(token::right_paren, "Expect ')' after function name.");

    consume(token::left_brace, "Expect '{' after function name.");
    parse_block();
    add_return_instruction();
    auto func = current_func_frame().func;
    pop_func_frame();
    add_instruction(op_constant{}, add_constant(func));
  }

  void add_return_instruction() noexcept {
    add_instruction(op_nil{});
    add_instruction(op_return{});
  }

  void parse_var_declaration() {
    const auto name = parse_variable("Expect variable name.");
    if (match(token::equal)) {
      parse_expression();
    } else {
      add_instruction(op_nil{});
    }
    consume(token::semicolon, "Expect ';' after variable declaration.");
    current_func_frame().define_variable(name, previous_->line);
  }

  size_t parse_variable(const std::string& message) {
    consume(token::identifier, message);
    current_func_frame().declare_variable(previous_->lexeme);
    if (current_func_frame().scope_depth > 0) {
      return 0;
    }
    return add_constant(vm_.main_heap.make_string(previous_->lexeme));
  }

  void parse_statement() {
    if (match(token::k_print)) {
      print_statement();
    } else if (match(token::k_for)) {
      parse_for();
    } else if (match(token::k_if)) {
      parse_if();
    } else if (match(token::k_return)) {
      parse_return();
    } else if (match(token::k_while)) {
      parse_while();
    } else if (match(token::left_brace)) {
      current_func_frame().begin_scope();
      parse_block();
      current_func_frame().end_scope(previous_->line);
    } else {
      expression_statement();
    }
  }

  void parse_for() {
    current_func_frame().begin_scope();
    consume(token::left_paren, "Expect '(' after 'for'.");
    if (match(token::semicolon)) {
    } else if (match(token::k_var)) {
      parse_var_declaration();
    } else {
      expression_statement();
    }
    auto loop_start = current_func_frame().current_code_position();
    auto exit_jump = -1;
    if (!match(token::semicolon)) {
      parse_expression();
      consume(token::semicolon, "Expect ';' after loop condition.");
      exit_jump = add_instruction(op_jump_if_false{});
      add_instruction(op_pop{});
    }

    if (!match(token::right_paren)) {
      auto body_jump = add_instruction(op_jump{});
      auto increament_start = current_func_frame().current_code_position();
      parse_expression();
      add_instruction(op_pop{});
      consume(token::right_paren, "Expect ')' after for clauses.");
      add_instruction(op_loop{},
                      current_func_frame().code_distance_from(loop_start));
      loop_start = increament_start;
      current_func_frame().patch_jump(body_jump);
    }

    parse_statement();
    add_instruction(op_loop{},
                    current_func_frame().code_distance_from(loop_start));
    if (exit_jump != -1) {
      current_func_frame().patch_jump(exit_jump);
      add_instruction(op_pop{});
    }
    current_func_frame().end_scope(previous_->line);
  }

  void parse_if() {
    consume(token::left_paren, "Expect '(' after 'if'.");
    parse_expression();
    consume(token::right_paren, "Expect ')' after condition.");

    const auto then_jump_index = add_instruction(op_jump_if_false{});
    add_instruction(op_pop{});
    parse_statement();
    const auto else_jump_index = add_instruction(op_jump{});

    current_func_frame().patch_jump(then_jump_index);
    add_instruction(op_pop{});
    if (match(token::k_else)) {
      parse_statement();
    }
    current_func_frame().patch_jump(else_jump_index);
  }

  void parse_return() {
    if (!current_func_frame().func->name) {
      throw compile_error{"Cannot return from top-level code."};
    }
    if (match(token::semicolon)) {
      add_return_instruction();
    } else {
      parse_expression();
      consume(token::semicolon, "Expect ';' after return value.");
      add_instruction(op_return{});
    }
  }

  void parse_while() {
    const auto loop_start = current_func_frame().current_code_position();
    consume(token::left_paren, "Expect '(' after 'while'.");
    parse_expression();
    consume(token::right_paren, "Expect ')' after condition.");
    const auto exit_jump_index = add_instruction(op_jump_if_false{}, 0);
    add_instruction(op_pop{});
    parse_statement();
    add_instruction(op_loop{},
                    current_func_frame().code_distance_from(loop_start));
    current_func_frame().patch_jump(exit_jump_index);
    add_instruction(op_pop{});
  }

  void parse_block() {
    while (!check(token::right_brace) && !check(token::eof)) {
      parse_declaration();
    }
    consume(token::right_brace, "Expect '}' after block.");
  }

  void print_statement() {
    parse_expression();
    consume(token::semicolon, "Expect ';' after value.");
    add_instruction(op_print{});
  }

  void expression_statement() {
    parse_expression();
    consume(token::semicolon, "Expect ';' after value.");
    add_instruction(op_pop{});
  }

  void parse_expression() { parse_precedence(precedence::p_assignment); }

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

  void parse_call(bool) { add_instruction(op_call{}, argument_list()); }

  size_t argument_list() {
    size_t count = 0;
    if (!check(token::right_paren)) {
      do {
        parse_expression();
        if (count == max_function_parameters) {
          throw runtime_error{"Cannot have more than " +
                              std::to_string(max_function_parameters) +
                              " arguments."};
        }
        ++count;
      } while (match(token::comma));
    }
    consume(token::right_paren, "Expect ')' after arguments.");
    return count;
  }

  void grouping(bool) {
    parse_expression();
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

  void add_variable(bool can_assign) {
    auto index_local = current_func_frame().resolve_local(previous_->lexeme);
    int index_global = -1;
    if (index_local == -1) {
      index_global = add_constant(vm_.main_heap.make_string(previous_->lexeme));
    }
    if (can_assign && match(token::equal)) {
      parse_expression();
      if (index_local == -1) {
        add_instruction(op_set_global{}, index_global);
      } else {
        add_instruction(op_set_local{}, index_local);
      }
    } else {
      if (index_local == -1) {
        add_instruction(op_get_global{}, index_global);
      } else {
        add_instruction(op_get_local{}, index_local);
      }
    }
  }

  void add_number_constant(bool) {
    add_instruction(op_constant{}, add_constant(std::stod(previous_->lexeme)));
  }
  void add_string_constant(bool) {
    add_instruction(op_constant{},
                    add_constant(vm_.main_heap.make_string(previous_->lexeme)));
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
    current_func_frame().patch_jump(end_jump_index);
  }

  void parse_or(bool) {
    const auto else_jump_index = add_instruction(op_jump_if_false{}, 0);
    const auto end_jump_index = add_instruction(op_jump{}, 0);
    current_func_frame().patch_jump(else_jump_index);
    add_instruction(op_pop{});
    parse_precedence(precedence::p_or);
    current_func_frame().patch_jump(end_jump_index);
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

  template <typename Opcode>
  size_t add_instruction(Opcode opcode, oprand_t oprand = 0) noexcept {
    return current_func_frame().add_instruction(previous_->line, opcode,
                                                oprand);
  }
  template <typename... Args>
  size_t add_constant(Args&&... args) noexcept {
    return current_func_frame().add_constant(std::forward<Args>(args)...);
  }

  struct local {
    local(std::string n, int d) noexcept : name{std::move(n)}, depth{d} {}

    std::string name;
    int depth;
  };
  using local_vector = std::vector<local>;

  struct func_frame {
    explicit func_frame(function* f, int depth) noexcept
        : func{f}, scope_depth{depth} {}

    void declare_variable(const std::string& name) {
      if (scope_depth > 0) {
        for (auto it = locals.crbegin(); it != locals.crend(); ++it) {
          if (it->depth != -1 && it->depth < scope_depth) {
            break;
          }
          if (it->name == name) {
            throw compile_error{"Variable '" + name +
                                "' already declared in this scope."};
          }
        }
        locals.emplace_back(name, -1);
      }
    }

    void define_variable(size_t name, int line) noexcept {
      if (scope_depth > 0) {
        latest_local_initialized();
      } else {
        add_instruction(line, op_define_global{}, name);
      }
    }

    void latest_local_initialized() noexcept {
      if (scope_depth > 0) {
        ENSURES(!locals.empty());
        locals.back().depth = scope_depth;
      }
    }

    int resolve_local(const std::string& name) const {
      for (int i = locals.size() - 1; i >= 0; --i) {
        if (locals[i].name == name) {
          if (locals[i].depth != -1) {
            return i;
          }
          throw compile_error{
              "Cannot read local variable in its own initializer."};
        }
      }
      return -1;
    }

    template <typename Opcode>
    size_t add_instruction(int line, Opcode opcode,
                           oprand_t oprand = 0) noexcept {
      return func->code.add_instruction(line, opcode, oprand);
    }
    template <typename... Args>
    size_t add_constant(Args&&... args) noexcept {
      return func->code.add_constant(std::forward<Args>(args)...);
    }

    void patch_jump(size_t jump) noexcept {
      func->code.set_oprand(jump, current_code_position() - (jump + 1));
    }
    size_t current_code_position() const noexcept {
      return func->code.instructions().size();
    }
    size_t code_distance_from(size_t pos) const noexcept {
      return current_code_position() - pos + 1;
    }

    void push_local() noexcept { locals.emplace_back("", 0); }

    void begin_scope() noexcept { ++scope_depth; }
    void end_scope(int line) noexcept {
      --scope_depth;
      while (!locals.empty() && locals.back().depth > scope_depth) {
        add_instruction(line, op_pop{});
        locals.pop_back();
      }
    }

    function* func = nullptr;
    std::vector<local> locals;
    int scope_depth;
  };
  using func_frame_vector = std::vector<func_frame>;

  void make_func_frame(const string* name, int depth) noexcept {
    func_frames.emplace_back(vm_.main_heap.make_object<function>(name), depth);
  }
  void pop_func_frame() noexcept { func_frames.pop_back(); }
  func_frame& current_func_frame() noexcept { return func_frames.back(); }

  friend precedence::rules_generator<compiler>;
  constexpr static precedence::rules<compiler> p_rules_ =
      precedence::rules_generator<compiler>::make_rules();

  constexpr static int max_function_parameters = 255;

  virtual_machine& vm_;
  func_frame_vector func_frames;
  token_vector tokens_;
  token_vector::const_iterator current_;
  token_vector::const_iterator previous_;
};

}  // namespace lox

#endif

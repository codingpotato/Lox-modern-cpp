#ifndef LOX_COMPILER_H
#define LOX_COMPILER_H

#include <array>
#include <string>

#include "chunk.h"
#include "exception.h"
#include "heap.h"
#include "scanner.h"

namespace lox {

namespace precedence {

enum Type {
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
using Parse_func = void (Compiler::*)(bool can_assign);

template <typename Compiler>
struct Rule {
  Parse_func<Compiler> prefix;
  Parse_func<Compiler> infix;
  Type precedence;
};

template <typename Compiler>
using Rules = std::array<Rule<Compiler>, static_cast<size_t>(Token::eof) + 1>;

template <typename Compiler>
struct Rules_generator {
  static constexpr Rules<Compiler> make_rules() noexcept {
    struct Element {
      Token::Type type;
      Rule<Compiler> rule_of_type;
    };
    constexpr Element elements[] = {
        {Token::left_paren,
         {&Compiler::parse_grouping, &Compiler::parse_call, p_call}},
        {Token::minus,
         {&Compiler::parse_unary, &Compiler::parse_binary, p_term}},
        {Token::plus, {nullptr, &Compiler::parse_binary, p_term}},
        {Token::slash, {nullptr, &Compiler::parse_binary, p_factor}},
        {Token::star, {nullptr, &Compiler::parse_binary, p_factor}},
        {Token::bang, {&Compiler::parse_unary, nullptr, p_none}},
        {Token::bang_equal, {nullptr, &Compiler::parse_binary, p_equality}},
        {Token::equal_equal, {nullptr, &Compiler::parse_binary, p_equality}},
        {Token::greater, {nullptr, &Compiler::parse_binary, p_comparison}},
        {Token::greater_equal,
         {nullptr, &Compiler::parse_binary, p_comparison}},
        {Token::less, {nullptr, &Compiler::parse_binary, p_comparison}},
        {Token::less_equal, {nullptr, &Compiler::parse_binary, p_comparison}},
        {Token::identifier, {&Compiler::add_variable, nullptr, p_none}},
        {Token::number, {&Compiler::add_number_constant, nullptr, p_none}},
        {Token::string, {&Compiler::add_string_constant, nullptr, p_none}},
        {Token::k_and, {nullptr, &Compiler::parse_and, p_and}},
        {Token::k_or, {nullptr, &Compiler::parse_or, p_or}},
        {Token::k_false, {&Compiler::add_literal, nullptr, p_none}},
        {Token::k_nil, {&Compiler::add_literal, nullptr, p_none}},
        {Token::k_true, {&Compiler::add_literal, nullptr, p_none}},
    };
    Rules<Compiler> rules{};
    for (auto it = std::cbegin(elements); it != std::end(elements); ++it) {
      rules[static_cast<size_t>(it->type)] = it->rule_of_type;
    }
    return rules;
  };
};

}  // namespace precedence

class Compiler {
 public:
  explicit Compiler(Heap &heap) noexcept : heap{&heap} {}

  Function *compile(Token_vector ts) {
    make_func_frame("", 0);
    tokens = std::move(ts);
    current = tokens.cbegin();
    while (!match(Token::eof)) {
      parse_declaration();
    }
    add_return_instruction();
    EXPECTS(func_frames.size() == 1)
    return func_frames.back().func;
  }

  template <typename Visitor>
  void for_each_func(Visitor &&visitor) const noexcept {
    for (const auto &func_frame : func_frames) {
      visitor(func_frame.func);
    }
  }

 private:
  void parse_declaration() {
    if (match(Token::k_func)) {
      parse_func_declaration();
    } else if (match(Token::k_var)) {
      parse_var_declaration();
    } else {
      parse_statement();
    }
  }

  void parse_func_declaration() {
    const auto name = parse_variable("Expect function name.");
    current_func_frame->initial_latest_local();
    parse_function();
    current_func_frame->define_variable(name, previous->line);
  }

  void parse_function() {
    make_func_frame(previous->lexeme, 1);
    current_func_frame->begin_scope();
    consume(Token::left_paren, "Expect '(' after function name.");

    parse_parameters();

    consume(Token::left_brace, "Expect '{' before function body.");
    parse_block();

    add_return_instruction();
    const auto func = current_func_frame->func;
    const auto upvalues = std::move(current_func_frame->upvalues);
    pop_func_frame();
    add<instruction::Closure>(add_constant(func), upvalues);
  }

  void parse_parameters() {
    if (!check(Token::right_paren)) {
      do {
        auto func = current_func_frame->func;
        func->inc_arity();
        if (func->get_arity() > max_function_parameters) {
          throw make_compile_error("Cannot have more than " +
                                       std::to_string(max_function_parameters) +
                                       " parameters.",
                                   *current);
        }
        auto parameter = parse_variable("Expect parameter name.");
        current_func_frame->define_variable(parameter, previous->line);
      } while (match(Token::comma));
    }
    consume(Token::right_paren, "Expect ')' after parameters.");
  }

  void parse_var_declaration() {
    const auto name = parse_variable("Expect variable name.");
    if (match(Token::equal)) {
      parse_expression();
    } else {
      add<instruction::Nil>();
    }
    consume(Token::semicolon, "Expect ';' after variable declaration.");
    current_func_frame->define_variable(name, previous->line);
  }

  size_t parse_variable(const std::string &message) {
    consume(Token::identifier, message);
    current_func_frame->declare_variable(*previous);
    if (current_func_frame->scope_depth > 0) {
      return 0;
    }
    return add_constant(heap->make_string(previous->lexeme));
  }

  void parse_statement() {
    if (match(Token::k_print)) {
      parse_print();
    } else if (match(Token::k_for)) {
      parse_for();
    } else if (match(Token::k_if)) {
      parse_if();
    } else if (match(Token::k_return)) {
      parse_return();
    } else if (match(Token::k_while)) {
      parse_while();
    } else if (match(Token::left_brace)) {
      current_func_frame->begin_scope();
      parse_block();
      current_func_frame->end_scope(previous->line);
    } else {
      parse_expression_statement();
    }
  }

  void parse_for() {
    current_func_frame->begin_scope();
    consume(Token::left_paren, "Expect '(' after 'for'.");
    if (match(Token::k_var)) {
      parse_var_declaration();
    } else if (!match(Token::semicolon)) {
      parse_expression_statement();
    }
    auto loop_start = current_code_size();
    auto exit_jump = -1;
    if (!match(Token::semicolon)) {
      parse_expression();
      consume(Token::semicolon, "Expect ';' after loop condition.");
      exit_jump = add<instruction::Jump_if_false>(0);
      add<instruction::Pop>();
    }

    if (!match(Token::right_paren)) {
      const auto body_jump = add<instruction::Jump>(0);
      const auto increament_start = current_code_size();
      parse_expression();
      add<instruction::Pop>();
      consume(Token::right_paren, "Expect ')' after for clauses.");
      emit_loop_from(loop_start);
      loop_start = increament_start;
      patch_jump(body_jump);
    }

    parse_statement();
    emit_loop_from(loop_start);
    if (exit_jump != -1) {
      patch_jump(exit_jump);
      add<instruction::Pop>();
    }
    current_func_frame->end_scope(previous->line);
  }

  void parse_if() {
    consume(Token::left_paren, "Expect '(' after 'if'.");
    parse_expression();
    consume(Token::right_paren, "Expect ')' after condition.");

    const auto then_jump_index = add<instruction::Jump_if_false>(0);
    add<instruction::Pop>();
    parse_statement();
    const auto else_jump_index = add<instruction::Jump>(0);

    patch_jump(then_jump_index);
    add<instruction::Pop>();
    if (match(Token::k_else)) {
      parse_statement();
    }
    patch_jump(else_jump_index);
  }

  void parse_return() {
    if (!current_func_frame->func->name) {
      throw make_compile_error("Cannot return from top-level code.");
    }
    if (match(Token::semicolon)) {
      add_return_instruction();
    } else {
      parse_expression();
      consume(Token::semicolon, "Expect ';' after return value.");
      add<instruction::Return>();
    }
  }

  void parse_while() {
    const auto loop_start = current_code_size();
    consume(Token::left_paren, "Expect '(' after 'while'.");
    parse_expression();
    consume(Token::right_paren, "Expect ')' after condition.");
    const auto exit_jump_index = add<instruction::Jump_if_false>(0);
    add<instruction::Pop>();
    parse_statement();
    emit_loop_from(loop_start);
    patch_jump(exit_jump_index);
    add<instruction::Pop>();
  }

  void parse_block() {
    while (!check(Token::right_brace) && !check(Token::eof)) {
      parse_declaration();
    }
    consume(Token::right_brace, "Expect '}' after block.");
  }

  void parse_print() {
    parse_expression();
    consume(Token::semicolon, "Expect ';' after value.");
    add<instruction::Print>();
  }

  void parse_expression_statement() {
    parse_expression();
    consume(Token::semicolon, "Expect ';' after value.");
    add<instruction::Pop>();
  }

  void parse_expression() { parse_precedence(precedence::p_assignment); }

  void parse_binary(bool) {
    const auto op_type = previous->type;
    parse_precedence(
        static_cast<precedence::Type>(p_rules_[op_type].precedence + 1));
    switch (op_type) {
      case Token::bang_equal:
        add<instruction::Equal>();
        add<instruction::Not>();
        break;
      case Token::equal_equal:
        add<instruction::Equal>();
        break;
      case Token::greater:
        add<instruction::Greater>();
        break;
      case Token::greater_equal:
        add<instruction::Less>();
        add<instruction::Not>();
        break;
      case Token::less:
        add<instruction::Less>();
        break;
      case Token::less_equal:
        add<instruction::Greater>();
        add<instruction::Not>();
        break;
      case Token::plus:
        add<instruction::Add>();
        break;
      case Token::minus:
        add<instruction::Subtract>();
        break;
      case Token::star:
        add<instruction::Multiply>();
        break;
      case Token::slash:
        add<instruction::Divide>();
        break;
      default:
        EXPECTS(false);
    }
  }

  void parse_call(bool) { add<instruction::Call>(parse_argument_list()); }

  size_t parse_argument_list() {
    size_t count = 0;
    if (!check(Token::right_paren)) {
      do {
        parse_expression();
        if (count == max_function_parameters) {
          throw make_compile_error("Cannot have more than " +
                                   std::to_string(max_function_parameters) +
                                   " arguments.");
        }
        ++count;
      } while (match(Token::comma));
    }
    consume(Token::right_paren, "Expect ')' after arguments.");
    return count;
  }

  void parse_grouping(bool) {
    parse_expression();
    consume(Token::right_paren, "Expect ')' after expression.");
  }

  void parse_unary(bool) {
    auto op_type = previous->type;
    parse_precedence(precedence::p_unary);
    switch (op_type) {
      case Token::bang:
        add<instruction::Not>();
        break;
      case Token::minus:
        add<instruction::Negate>();
        break;
      default:
        break;
    }
  }

  void parse_and(bool) {
    const auto end_jump_index = add<instruction::Jump_if_false>(0);
    add<instruction::Pop>();
    parse_precedence(precedence::p_and);
    patch_jump(end_jump_index);
  }

  void parse_or(bool) {
    const auto else_jump_index = add<instruction::Jump_if_false>(0);
    const auto end_jump_index = add<instruction::Jump>(0);
    patch_jump(else_jump_index);
    add<instruction::Pop>();
    parse_precedence(precedence::p_or);
    patch_jump(end_jump_index);
  }

  void parse_precedence(precedence::Type precedence) {
    advance();
    const auto prefix = p_rules_[previous->type].prefix;
    if (prefix == nullptr) {
      throw make_compile_error("Expect expression.");
    }
    const auto can_assign = precedence <= precedence::p_assignment;
    (this->*prefix)(can_assign);
    while (static_cast<int>(precedence) <=
           static_cast<int>(p_rules_[current->type].precedence)) {
      advance();
      const auto infix = p_rules_[previous->type].infix;
      (this->*infix)(can_assign);
    }

    if (can_assign && match(Token::equal)) {
      throw make_compile_error("Invalid assignment target.");
    }
  }

  struct Variable_get {};
  struct Variable_set {};
  enum class Variable_type { local, upvalue, global };

  void add_variable(bool can_assign) {
    auto type = Variable_type::local;
    auto index = current_func_frame->resolve_local(*previous);
    if (index == -1) {
      index = resolve_upvalue(previous->lexeme, func_frames.size() - 1);
      if (index != -1) {
        type = Variable_type::upvalue;
      } else {
        type = Variable_type::global;
        index = add_constant(heap->make_string(previous->lexeme));
      }
    }
    if (can_assign && match(Token::equal)) {
      parse_expression();
      add_variable<Variable_set>(type, index);
    } else {
      add_variable<Variable_get>(type, index);
    }
  }

  template <typename Variable_get_set>
  void add_variable(Variable_type type, size_t index) {
    if constexpr (std::is_same_v<Variable_get_set, Variable_get>) {
      switch (type) {
        case Variable_type::global:
          add<instruction::Get_global>(index);
          break;
        case Variable_type::local:
          add<instruction::Get_local>(index);
          break;
        case Variable_type::upvalue:
          add<instruction::Get_upvalue>(index);
          break;
      }
    } else {
      switch (type) {
        case Variable_type::global:
          add<instruction::Set_global>(index);
          break;
        case Variable_type::local:
          add<instruction::Set_local>(index);
          break;
        case Variable_type::upvalue:
          add<instruction::Set_upvalue>(index);
          break;
      }
    }
  }

  int resolve_upvalue(const std::string &name, size_t frame_index) {
    if (frame_index > 0) {
      ENSURES(frame_index < func_frames.size());
      auto &previous_frame = func_frames[frame_index - 1];
      if (const auto local = previous_frame.resolve_local(*previous);
          local != -1) {
        previous_frame.locals[local].is_captured = true;
        return func_frames[frame_index].add_upvalue(local, true, *previous);
      }
      if (const auto upvalue = resolve_upvalue(name, frame_index - 1);
          upvalue != -1) {
        return func_frames[frame_index].add_upvalue(upvalue, false, *previous);
      }
    }
    return -1;
  }

  void add_number_constant(bool) {
    add<instruction::Constant>(add_constant(std::stod(previous->lexeme)));
  }
  void add_string_constant(bool) {
    add<instruction::Constant>(
        add_constant(heap->make_string(previous->lexeme)));
  }

  void add_literal(bool) {
    switch (previous->type) {
      case Token::k_nil:
        add<instruction::Nil>();
        break;
      case Token::k_false:
        add<instruction::False>();
        break;
      case Token::k_true:
        add<instruction::True>();
        break;
      default:
        throw make_compile_error("Unknow literal.");
    }
  }

  void advance() noexcept {
    previous = current;
    ++current;
  }

  void consume(Token::Type type, const std::string &message) {
    if (current->type == type) {
      advance();
    } else {
      throw make_compile_error(message, true);
    }
  }

  bool check(Token::Type type) noexcept { return current->type == type; }

  bool match(Token::Type type) noexcept {
    if (check(type)) {
      advance();
      return true;
    }
    return false;
  }

  void emit_loop_from(size_t pos) {
    const auto distance = current_code_size() + instruction::Loop::size - pos;
    if (distance <= instruction::Loop::operand_max) {
      add<instruction::Loop>(distance);
    } else {
      throw make_compile_error("Loop body too large.", *previous);
    }
  }

  void add_return_instruction() noexcept {
    add<instruction::Nil>();
    add<instruction::Return>();
  }

  template <typename Instruction>
  size_t add() noexcept {
    ENSURES(!func_frames.empty() && func_frames.back().func);
    auto &chunk = func_frames.back().func->get_chunk();
    return chunk.add<Instruction>(previous->line);
  }

  template <typename Instruction>
  size_t add(size_t operand) noexcept {
    ENSURES(!func_frames.empty() && func_frames.back().func);
    auto &chunk = func_frames.back().func->get_chunk();
    return chunk.add<Instruction>(operand, previous->line);
  }

  template <typename Instruction>
  size_t add(size_t operand,
             const typename Instruction::Upvalue_vector &upvalues) noexcept {
    ENSURES(!func_frames.empty() && func_frames.back().func);
    auto &chunk = func_frames.back().func->get_chunk();
    return chunk.add<Instruction>(operand, upvalues, previous->line);
  }

  template <typename... Args>
  size_t add_constant(Args &&... args) {
    ENSURES(!func_frames.empty() && func_frames.back().func);
    auto &chunk = func_frames.back().func->get_chunk();
    const auto index = chunk.add_constant(std::forward<Args>(args)...);
    if (index <= instruction::Constant_instr::operand_max) {
      return index;
    }
    throw make_compile_error("Too many constants in one chunk.", *previous);
  }

  void patch_jump(size_t jump) noexcept {
    auto &chunk = current_func_frame->get_chunk();
    chunk.patch_jump(jump,
                     chunk.code_size() - jump - instruction::Jump_instr::size);
  }

  size_t current_code_size() const noexcept {
    return current_func_frame->get_chunk().code_size();
  }

  struct Local {
    Local(std::string name, int depth) noexcept
        : name{std::move(name)}, depth{depth} {}

    std::string name;
    int depth;
    bool is_captured = false;
  };
  using Local_vector = std::vector<Local>;

  struct Func_frame {
    Func_frame(Function *func, int depth) noexcept
        : func{func}, scope_depth{depth} {
      ENSURES(func);
      locals.emplace_back("function object", depth);
    }

    Chunk &get_chunk() noexcept { return func->get_chunk(); }

    void declare_variable(const Token &token) {
      const auto &name = token.lexeme;
      if (scope_depth > 0) {
        for (auto it = locals.crbegin(); it != locals.crend(); ++it) {
          if (it->depth != -1 && it->depth < scope_depth) {
            break;
          }
          if (it->name == name) {
            throw make_compile_error(
                "Variable with this name already declared in this scope.",
                token);
          }
        }
        add_local(name, token);
      }
    }

    void define_variable(size_t name_constant, int line) noexcept {
      if (scope_depth > 0) {
        initial_latest_local();
      } else {
        get_chunk().add<instruction::Define_global>(name_constant, line);
      }
    }

    void initial_latest_local() noexcept {
      if (scope_depth > 0) {
        ENSURES(!locals.empty());
        locals.back().depth = scope_depth;
      }
    }

    int resolve_local(const Token &token) const {
      const auto &name = token.lexeme;
      for (int i = locals.size() - 1; i >= 0; --i) {
        if (locals[i].name == name) {
          if (locals[i].depth != -1) {
            return i;
          }
          throw make_compile_error(
              "Cannot read local variable in its own initializer.", token);
        }
      }
      return -1;
    }

    void add_local(std::string name, const Token &token) {
      if (locals.size() <= UINT8_MAX) {
        locals.emplace_back(std::move(name), -1);
      } else {
        throw make_compile_error("Too many local variables in function.",
                                 token);
      }
    }

    void begin_scope() noexcept { ++scope_depth; }
    void end_scope(int line) noexcept {
      --scope_depth;
      while (!locals.empty() && locals.back().depth > scope_depth) {
        if (const auto &local = locals.back(); local.is_captured) {
          get_chunk().add<instruction::Close_upvalue>(line);
        } else {
          get_chunk().add<instruction::Pop>(line);
        }
        locals.pop_back();
      }
    }

    size_t add_upvalue(size_t index, bool is_local, const Token &token) {
      for (size_t i = 0; i < upvalues.size(); ++i) {
        if (upvalues[i].index == index && upvalues[i].is_local == is_local) {
          return i;
        }
      }
      if (upvalues.size() <= UINT8_MAX) {
        upvalues.emplace_back(index, is_local);
        func->upvalue_count = upvalues.size();
        return func->upvalue_count - 1;
      }
      throw make_compile_error("Too many closure variables in function.",
                               token);
    }

    Function *func;
    Local_vector locals;
    instruction::Closure::Upvalue_vector upvalues;
    int scope_depth;
  };

  using Func_frame_vector = std::vector<Func_frame>;

  void make_func_frame(const std::string &name, int depth) noexcept {
    auto func = heap->make_object<Function>();
    func_frames.emplace_back(func, depth);
    if (!name.empty()) {
      func->name = heap->make_string(name);
    }
    current_func_frame = &func_frames.back();
  }

  void pop_func_frame() noexcept {
    func_frames.pop_back();
    current_func_frame = func_frames.empty() ? nullptr : &func_frames.back();
  }

  static Compile_error make_compile_error(const std::string &message,
                                          const Token &token) noexcept;

  Compile_error make_compile_error(const std::string &message,
                                   bool from_current = false) const noexcept;

  friend precedence::Rules_generator<Compiler>;
  constexpr static precedence::Rules<Compiler> p_rules_ =
      precedence::Rules_generator<Compiler>::make_rules();

  constexpr static int max_function_parameters = UINT8_MAX;

  Heap *heap;
  Func_frame_vector func_frames;
  Func_frame *current_func_frame = nullptr;
  Token_vector tokens;
  Token_vector::const_iterator current;
  Token_vector::const_iterator previous;
};

}  // namespace lox

#endif

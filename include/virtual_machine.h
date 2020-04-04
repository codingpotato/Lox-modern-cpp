#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <list>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "chunk.h"
#include "exception.h"
#include "hash_table.h"
#include "heap.h"
#include "instruction.h"
#include "object.h"
#include "stack.h"
#include "value.h"

namespace lox {

class virtual_machine {
 public:
  explicit virtual_machine(std::ostream& os) noexcept : out_{os} {}

  inline void interpret(function* func);

  template <typename Opcode>
  void handle(oprand_t) {
    throw internal_error{"Need implement for " + Opcode::name};
  }

  heap main_heap;

 private:
  struct call_frame {
    call_frame() noexcept {}
    call_frame(function* f, size_t start = 0) noexcept
        : func{f}, start_of_stack{start} {}

    function* func = nullptr;
    size_t ip = 0;
    size_t start_of_stack = 0;
  };

  template <typename Func>
  void binary(Func&& func) {
    auto right = stack_.pop();
    auto left = stack_.pop();
    stack_.push(std::forward<Func>(func)(left, right));
  }

  call_frame& current_frame() noexcept { return call_frames.peek(); }
  chunk& current_code() noexcept { return current_frame().func->code; }

  constexpr static size_t max_frame_size = 64;
  constexpr static size_t max_stack_size = max_frame_size * 1024;

  std::ostream& out_;
  stack<call_frame, max_frame_size> call_frames;
  stack<value, max_stack_size> stack_;
  hash_table globals_;
};

template <>
inline void virtual_machine::handle<op_constant>(oprand_t oprand) {
  ENSURES(oprand < current_code().constant_size());
  stack_.push(current_code().constant_at(oprand));
}

template <>
inline void virtual_machine::handle<op_nil>(oprand_t) {
  stack_.push();
}

template <>
inline void virtual_machine::handle<op_false>(oprand_t) {
  stack_.push(false);
}

template <>
inline void virtual_machine::handle<op_true>(oprand_t) {
  stack_.push(true);
}

template <>
inline void virtual_machine::handle<op_pop>(oprand_t) {
  stack_.pop();
}

template <>
inline void virtual_machine::handle<op_get_local>(oprand_t oprand) {
  stack_.push(stack_[call_frames.peek().start_of_stack + oprand]);
}

template <>
inline void virtual_machine::handle<op_set_local>(oprand_t oprand) {
  stack_[call_frames.peek().start_of_stack + oprand] = stack_.peek();
}

template <>
inline void virtual_machine::handle<op_get_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constant_size());
  const auto constant = current_code().constant_at(oprand);
  const auto name = constant.as<object*>()->as<string>();
  if (auto value = globals_.get_if(name); value != nullptr) {
    stack_.push(*value);
  } else {
    throw runtime_error{"Undefined variable: " + name->std_string()};
  }
}

template <>
inline void virtual_machine::handle<op_define_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constant_size());
  const auto constant = current_code().constant_at(oprand);
  const auto name = constant.as<object*>()->as<string>();
  auto str = main_heap.make_string(name->std_string());
  globals_.insert(str, stack_.pop());
}

template <>
inline void virtual_machine::handle<op_set_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constant_size());
  const auto constant = current_code().constant_at(oprand);
  const auto name = constant.as<object*>()->as<string>();
  if (!globals_.set(name, stack_.peek())) {
    throw runtime_error{"Undefined variable: " + name->std_string()};
  }
}

template <>
inline void virtual_machine::handle<op_equal>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left == right; });
}

template <>
inline void virtual_machine::handle<op_greater>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left > right; });
}

template <>
inline void virtual_machine::handle<op_less>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left < right; });
}

template <>
inline void virtual_machine::handle<op_add>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left + right; });
}

template <>
inline void virtual_machine::handle<op_subtract>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left - right; });
}

template <>
inline void virtual_machine::handle<op_multiply>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left * right; });
}

template <>
inline void virtual_machine::handle<op_divide>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left / right; });
}

template <>
inline void virtual_machine::handle<op_not>(oprand_t) {
  if (stack_.peek().is<bool>()) {
    stack_.push(!stack_.pop().as<bool>());
  } else {
    throw runtime_error{"Operand must be a boolean."};
  }
}

template <>
inline void virtual_machine::handle<op_negate>(oprand_t) {
  if (stack_.peek().is<double>()) {
    const auto operand = stack_.pop();
    const auto result = operand.is<nil>() || !operand.as<bool>();
    stack_.push(result);
  } else {
    throw runtime_error{"Operand must be a number."};
  }
}

template <>
inline void virtual_machine::handle<op_print>(oprand_t) {
  ENSURES(!stack_.empty());
  out_ << stack_.pop().repr() << "\n";
}

template <>
inline void virtual_machine::handle<op_jump>(oprand_t oprand) {
  call_frames.peek().ip += oprand;
}

template <>
inline void virtual_machine::handle<op_jump_if_false>(oprand_t oprand) {
  ENSURES(!stack_.empty());
  if (!stack_.peek().as<bool>()) {
    call_frames.peek().ip += oprand;
  }
}

template <>
inline void virtual_machine::handle<op_loop>(oprand_t oprand) {
  call_frames.peek().ip -= oprand;
}

template <>
inline void virtual_machine::handle<op_return>(oprand_t) {}

#define SWITCH_CASE_(opcode, has_oprand_value) \
  case opcode::id:                             \
    handle<opcode>(instr.oprand());            \
    break;

inline void virtual_machine::interpret(function* func) {
  stack_.push(func);
  call_frames.push(func);
  while (call_frames.peek().ip < current_code().instruction_size()) {
    const auto& instr = current_code().instruction_at(call_frames.peek().ip++);
    switch (instr.raw_opcode()) { OPCODES(SWITCH_CASE_) };
  }
}

}  // namespace lox

#endif

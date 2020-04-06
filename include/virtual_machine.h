#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <ostream>
#include <string>

#include "exception.h"
#include "hash_table.h"
#include "heap.h"
#include "instruction.h"
#include "native.h"
#include "object.h"
#include "stack.h"
#include "value.h"

namespace lox {

class virtual_machine {
 public:
  explicit virtual_machine(std::ostream& os) noexcept : out_{os} {
    register_natives(globals_, main_heap);
  }

  template <bool Debug = false>
  inline void interpret(const function* func) noexcept;

  template <typename Opcode>
  void handle(oprand_t) {}

  heap main_heap;

 private:
  struct call_frame {
    call_frame() noexcept {}
    call_frame(const function* f, size_t start = 0) noexcept
        : func{f}, start_of_stack{start} {}

    const function* func = nullptr;
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
  const chunk& current_code() noexcept { return current_frame().func->code; }

  void throw_undefined_variable(const string* name) const {
    throw runtime_error{"Undefined variable: " + name->std_string()};
  }

  void throw_incorrect_argument_count(int arity, int argument_count) const {
    throw runtime_error{"Expected " + std::to_string(arity) +
                        " arguments but got " + std::to_string(argument_count) +
                        "."};
  }

  constexpr static size_t max_frame_size = 64;
  constexpr static size_t max_stack_size = max_frame_size * 1024;

  std::ostream& out_;
  stack<call_frame, max_frame_size> call_frames;
  stack<value, max_stack_size> stack_;
  hash_table globals_;
};

template <>
inline void virtual_machine::handle<op_constant>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  stack_.push(current_code().constants()[oprand]);
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
  stack_.push(stack_[current_frame().start_of_stack + oprand]);
}

template <>
inline void virtual_machine::handle<op_set_local>(oprand_t oprand) {
  stack_[current_frame().start_of_stack + oprand] = stack_.peek();
}

template <>
inline void virtual_machine::handle<op_get_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  const auto constant = current_code().constants()[oprand];
  const auto name = constant.as_object()->as<string>();
  if (auto value = globals_.get_if(name); value != nullptr) {
    stack_.push(*value);
  } else {
    throw_undefined_variable(name);
  }
}

template <>
inline void virtual_machine::handle<op_define_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  const auto constant = current_code().constants()[oprand];
  const auto name = constant.as_object()->as<string>();
  auto str = main_heap.make_string(name->std_string());
  globals_.insert(str, stack_.pop());
}

template <>
inline void virtual_machine::handle<op_set_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  const auto constant = current_code().constants()[oprand];
  const auto name = constant.as_object()->as<string>();
  if (!globals_.set(name, stack_.peek())) {
    throw_undefined_variable(name);
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
  if (stack_.peek().is_bool()) {
    stack_.push(!stack_.pop().as_bool());
  } else {
    throw runtime_error{"Operand must be a boolean value."};
  }
}

template <>
inline void virtual_machine::handle<op_negate>(oprand_t) {
  if (stack_.peek().is_double()) {
    const auto operand = stack_.pop();
    const auto result = operand.is_nil() || !operand.as_bool();
    stack_.push(result);
  } else {
    throw runtime_error{"Operand must be a number value."};
  }
}

template <>
inline void virtual_machine::handle<op_print>(oprand_t) {
  ENSURES(!stack_.empty());
  out_ << to_string(stack_.pop()) << "\n";
}

template <>
inline void virtual_machine::handle<op_jump>(oprand_t oprand) {
  current_frame().ip += oprand;
}

template <>
inline void virtual_machine::handle<op_jump_if_false>(oprand_t oprand) {
  ENSURES(!stack_.empty());
  if (auto value = stack_.peek(); value.is_bool()) {
    if (!value.as_bool()) {
      current_frame().ip += oprand;
    }
  } else {
    throw runtime_error{"Operand must be a boolean value."};
  }
}

template <>
inline void virtual_machine::handle<op_loop>(oprand_t oprand) {
  current_frame().ip -= oprand;
}

template <>
inline void virtual_machine::handle<op_call>(oprand_t oprand) {
  int argument_count = oprand;
  if (auto v = stack_.peek(argument_count); v.is_object()) {
    auto obj = v.as_object();
    if (obj->is<function>()) {
      auto func = obj->as<function>();
      if (argument_count == func->arity) {
        call_frames.push(func, stack_.size() - argument_count - 1);
      } else {
        throw_incorrect_argument_count(func->arity, argument_count);
      }
    } else if (obj->is<Native_func>()) {
      auto func = obj->as<Native_func>();
      auto result =
          (*func)(argument_count, &stack_[stack_.size() - argument_count]);
      stack_.resize(stack_.size() - argument_count - 1);
      stack_.push(result);
    }
  }
}

template <>
inline void virtual_machine::handle<op_return>(oprand_t) {
  auto result = stack_.pop();
  auto stack_size = call_frames.peek().start_of_stack;
  call_frames.pop();
  if (call_frames.empty()) {
    stack_.pop();
  } else {
    stack_.resize(stack_size);
    stack_.push(result);
  }
}

#define SWITCH_CASE_(opcode, has_oprand_value) \
  case opcode::id:                             \
    handle<opcode>(instr.oprand());            \
    break;

template <bool Debug>
inline void virtual_machine::interpret(const function* func) noexcept {
  try {
    stack_.push(func);
    call_frames.push(func);
    while (!call_frames.empty() &&
           call_frames.peek().ip < current_code().instructions().size()) {
      const auto& instr =
          current_code().instructions()[call_frames.peek().ip++];
      switch (instr.raw_opcode()) { OPCODES(SWITCH_CASE_) };
      if constexpr (Debug) {
        for (size_t i = 0; i < stack_.size(); ++i) {
          out_ << to_string(stack_[i]) << " ";
        }
        out_ << "\n";
      }
    }
  } catch (exception& e) {
    out_ << e.what() << "\n";
    for (size_t distance = 0; distance < call_frames.size(); ++distance) {
      out_ << call_frames.peek(distance).func->to_string() << "\n";
    }
  }
}

}  // namespace lox

#endif

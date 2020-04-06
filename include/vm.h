#ifndef LOX_VM_H
#define LOX_VM_H

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

class Vm {
 public:
  explicit Vm(std::ostream& os) noexcept : out_{os} {
    register_natives(globals_, heap_);
  }

  Heap& heap() noexcept { return heap_; }

  template <bool Debug = false>
  inline void interpret(Function* func) noexcept;

  template <typename Opcode>
  void handle(oprand_t) {}

 private:
  struct Call_frame {
    Call_frame() noexcept {}
    Call_frame(Function* f, size_t start = 0) noexcept
        : func{f}, start_of_stack{start} {}

    Function* func = nullptr;
    size_t ip = 0;
    size_t start_of_stack = 0;
  };

  template <typename Func>
  void binary(Func&& func) {
    auto right = stack_.pop();
    auto left = stack_.pop();
    stack_.push(std::forward<Func>(func)(left, right));
  }

  Call_frame& current_frame() noexcept { return call_frames_.peek(); }
  chunk& current_code() noexcept { return current_frame().func->code(); }

  void throw_undefined_variable(const String* name) const {
    throw runtime_error{"Undefined variable: " + name->string()};
  }

  void throw_incorrect_argument_count(int arity, int argument_count) const {
    throw runtime_error{"Expected " + std::to_string(arity) +
                        " arguments but got " + std::to_string(argument_count) +
                        "."};
  }

  constexpr static size_t max_frame_size = 64;
  constexpr static size_t max_stack_size = max_frame_size * 1024;

  std::ostream& out_;
  Stack<Call_frame, max_frame_size> call_frames_;
  Stack<Value, max_stack_size> stack_;
  Hash_table globals_;
  Heap heap_;
};

template <>
inline void Vm::handle<op_constant>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  stack_.push(current_code().constants()[oprand]);
}

template <>
inline void Vm::handle<op_nil>(oprand_t) {
  stack_.push();
}

template <>
inline void Vm::handle<op_false>(oprand_t) {
  stack_.push(false);
}

template <>
inline void Vm::handle<op_true>(oprand_t) {
  stack_.push(true);
}

template <>
inline void Vm::handle<op_pop>(oprand_t) {
  stack_.pop();
}

template <>
inline void Vm::handle<op_get_local>(oprand_t oprand) {
  stack_.push(stack_[current_frame().start_of_stack + oprand]);
}

template <>
inline void Vm::handle<op_set_local>(oprand_t oprand) {
  stack_[current_frame().start_of_stack + oprand] = stack_.peek();
}

template <>
inline void Vm::handle<op_get_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  auto constant = current_code().constants()[oprand];
  auto name = constant.as_object()->as<String>();
  if (auto value = globals_.get_if(name); value != nullptr) {
    stack_.push(*value);
  } else {
    throw_undefined_variable(name);
  }
}

template <>
inline void Vm::handle<op_define_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  auto constant = current_code().constants()[oprand];
  auto name = constant.as_object()->as<String>();
  auto str = heap_.make_string(name->string());
  globals_.insert(str, stack_.pop());
}

template <>
inline void Vm::handle<op_set_global>(oprand_t oprand) {
  ENSURES(oprand < current_code().constants().size());
  auto constant = current_code().constants()[oprand];
  auto name = constant.as_object()->as<String>();
  if (!globals_.set(name, stack_.peek())) {
    throw_undefined_variable(name);
  }
}

template <>
inline void Vm::handle<op_equal>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left == right; });
}

template <>
inline void Vm::handle<op_greater>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left > right; });
}

template <>
inline void Vm::handle<op_less>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left < right; });
}

template <>
inline void Vm::handle<op_add>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left + right; });
}

template <>
inline void Vm::handle<op_subtract>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left - right; });
}

template <>
inline void Vm::handle<op_multiply>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left * right; });
}

template <>
inline void Vm::handle<op_divide>(oprand_t) {
  binary([](const auto& left, const auto& right) { return left / right; });
}

template <>
inline void Vm::handle<op_not>(oprand_t) {
  if (stack_.peek().is_bool()) {
    stack_.push(!stack_.pop().as_bool());
  } else {
    throw runtime_error{"Operand must be a boolean value."};
  }
}

template <>
inline void Vm::handle<op_negate>(oprand_t) {
  if (stack_.peek().is_double()) {
    const auto operand = stack_.pop();
    const auto result = operand.is_nil() || !operand.as_bool();
    stack_.push(result);
  } else {
    throw runtime_error{"Operand must be a number value."};
  }
}

template <>
inline void Vm::handle<op_print>(oprand_t) {
  ENSURES(!stack_.empty());
  out_ << to_string(stack_.pop()) << "\n";
}

template <>
inline void Vm::handle<op_jump>(oprand_t oprand) {
  current_frame().ip += oprand;
}

template <>
inline void Vm::handle<op_jump_if_false>(oprand_t oprand) {
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
inline void Vm::handle<op_loop>(oprand_t oprand) {
  current_frame().ip -= oprand;
}

template <>
inline void Vm::handle<op_call>(oprand_t oprand) {
  const auto argument_count = oprand;
  if (auto v = stack_.peek(argument_count); v.is_object()) {
    auto obj = v.as_object();
    if (obj->is<Function>()) {
      auto func = obj->as<Function>();
      if (argument_count == func->arity()) {
        call_frames_.push(func, stack_.size() - argument_count - 1);
      } else {
        throw_incorrect_argument_count(func->arity(), argument_count);
      }
    } else if (obj->is<Native_func>()) {
      const auto func = obj->as<Native_func>();
      const auto result =
          (*func)(argument_count, &stack_[stack_.size() - argument_count]);
      stack_.resize(stack_.size() - argument_count - 1);
      stack_.push(result);
    }
  }
}

template <>
inline void Vm::handle<op_return>(oprand_t) {
  auto result = stack_.pop();
  auto stack_size = call_frames_.peek().start_of_stack;
  call_frames_.pop();
  if (call_frames_.empty()) {
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
inline void Vm::interpret(Function* func) noexcept {
  try {
    stack_.push(func);
    call_frames_.push(func);
    while (!call_frames_.empty() &&
           call_frames_.peek().ip < current_code().instructions().size()) {
      const auto& instr =
          current_code().instructions()[call_frames_.peek().ip++];
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
    for (size_t distance = 0; distance < call_frames_.size(); ++distance) {
      out_ << call_frames_.peek(distance).func->to_string() << "\n";
    }
  }
}

}  // namespace lox

#endif

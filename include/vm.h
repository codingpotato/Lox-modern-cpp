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

  template <typename Instruction>
  void handle(const Instruction&) {
    EXPECTS(false);
  }

 private:
  struct Call_frame {
    Call_frame() noexcept {}
    Call_frame(Closure* c, size_t start = 0) noexcept
        : closure{c}, start_of_stack{start} {}

    Closure* closure = nullptr;
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

  void close_upvalues(const Value* last) noexcept {
    const auto& open_upvalues = heap_.get_open_upvalues();
    for (auto it = open_upvalues.begin(); it != open_upvalues.end(); ++it) {
      if (it->location >= last) {
        it->closed = *it->location;
        it->location = &it->closed;
      }
    }
  }

  void call_closure(Closure* closure, size_t argument_count) noexcept {
    if (!call_frames_.empty()) {
      current_frame().ip = ip;
    }
    call_frames_.push(closure, stack_.size() - argument_count - 1);
    const auto& chunk = closure->func()->chunk();
    code = &chunk.code();
    constants = &chunk.constants();
    ip = 0;
  }

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
  const Bytecode_vector* code = nullptr;
  const Value_vector* constants = nullptr;
  size_t ip = 0;
};

template <>
inline void Vm::handle(const instruction::Constant& constant) {
  ENSURES(constants != nullptr);
  ENSURES(constant.operand() < constants->size());
  stack_.push((*constants)[constant.operand()]);
}

template <>
inline void Vm::handle(const instruction::Nil&) {
  stack_.push();
}

template <>
inline void Vm::handle(const instruction::False&) {
  stack_.push(false);
}

template <>
inline void Vm::handle(const instruction::True&) {
  stack_.push(true);
}

template <>
inline void Vm::handle(const instruction::Pop&) {
  stack_.pop();
}

template <>
inline void Vm::handle(const instruction::Get_local& get_local) {
  stack_.push(stack_[current_frame().start_of_stack + get_local.operand()]);
}

template <>
inline void Vm::handle(const instruction::Set_local& set_local) {
  stack_[current_frame().start_of_stack + set_local.operand()] = stack_.peek();
}

template <>
inline void Vm::handle(const instruction::Get_global& get_global) {
  ENSURES(constants != nullptr);
  ENSURES(get_global.operand() < constants->size());
  auto constant = (*constants)[get_global.operand()];
  auto name = constant.as_object()->as<String>();
  if (auto value = globals_.get_if(name); value != nullptr) {
    stack_.push(*value);
  } else {
    throw_undefined_variable(name);
  }
}

template <>
inline void Vm::handle(const instruction::Define_global& define_global) {
  ENSURES(constants != nullptr);
  ENSURES(define_global.operand() < constants->size());
  auto constant = (*constants)[define_global.operand()];
  const auto name = constant.as_object()->as<String>();
  const auto str = heap_.make_string(name->string());
  globals_.insert(str, stack_.pop());
}

template <>
inline void Vm::handle(const instruction::Set_global& set_global) {
  ENSURES(constants != nullptr);
  ENSURES(set_global.operand() < constants->size());
  auto constant = (*constants)[set_global.operand()];
  const auto name = constant.as_object()->as<String>();
  if (!globals_.set(name, stack_.peek())) {
    throw_undefined_variable(name);
  }
}

template <>
inline void Vm::handle(const instruction::Get_upvalue& get_upvalue) {
  const auto slot = get_upvalue.operand();
  ENSURES(slot < current_frame().closure->upvalues.size());
  stack_.push(*current_frame().closure->upvalues[slot]->location);
}

template <>
inline void Vm::handle(const instruction::Set_upvalue& set_upvalue) {
  auto slot = set_upvalue.operand();
  ENSURES(slot < current_frame().closure->upvalues.size());
  *current_frame().closure->upvalues[slot]->location = stack_.peek();
}

template <>
inline void Vm::handle(const instruction::Equal&) {
  binary([](const auto& left, const auto& right) { return left == right; });
}

template <>
inline void Vm::handle(const instruction::Greater&) {
  binary([](const auto& left, const auto& right) { return left > right; });
}

template <>
inline void Vm::handle(const instruction::Less&) {
  binary([](const auto& left, const auto& right) { return left < right; });
}

template <>
inline void Vm::handle(const instruction::Add&) {
  binary([](const auto& left, const auto& right) { return left + right; });
}

template <>
inline void Vm::handle(const instruction::Subtract&) {
  binary([](const auto& left, const auto& right) { return left - right; });
}

template <>
inline void Vm::handle(const instruction::Multiply&) {
  binary([](const auto& left, const auto& right) { return left * right; });
}

template <>
inline void Vm::handle(const instruction::Divide&) {
  binary([](const auto& left, const auto& right) { return left / right; });
}

template <>
inline void Vm::handle(const instruction::Not&) {
  if (stack_.peek().is_bool()) {
    stack_.push(!stack_.pop().as_bool());
  } else {
    throw runtime_error{"Operand must be a boolean value."};
  }
}

template <>
inline void Vm::handle(const instruction::Negate&) {
  if (stack_.peek().is_double()) {
    stack_.push(-stack_.pop().as_double());
  } else {
    throw runtime_error{"Operand must be a number value."};
  }
}

template <>
inline void Vm::handle(const instruction::Print&) {
  ENSURES(!stack_.empty());
  out_ << to_string(stack_.pop()) << "\n";
}

template <>
inline void Vm::handle(const instruction::Jump& jump) {
  ip += jump.operand();
}

template <>
inline void Vm::handle(const instruction::Jump_if_false& jump_if_false) {
  ENSURES(!stack_.empty());
  if (auto value = stack_.peek(); value.is_bool()) {
    if (!value.as_bool()) {
      ip += jump_if_false.operand();
    }
  } else {
    throw runtime_error{"Operand must be a boolean value."};
  }
}

template <>
inline void Vm::handle(const instruction::Loop& loop) {
  ip -= loop.operand();
}

template <>
inline void Vm::handle(const instruction::Call& call) {
  const auto argument_count = call.operand();
  if (auto v = stack_.peek(argument_count); v.is_object()) {
    auto obj = v.as_object();
    if (obj->is<Closure>()) {
      auto closure = obj->as<Closure>();
      if (argument_count == closure->func()->arity()) {
        call_closure(closure, argument_count);
      } else {
        throw_incorrect_argument_count(closure->func()->arity(),
                                       argument_count);
      }
    } else if (obj->is<Native_func>()) {
      const auto func = obj->as<Native_func>();
      const auto result =
          (*func)(argument_count, argument_count > 0
                                      ? &stack_[stack_.size() - argument_count]
                                      : nullptr);
      stack_.resize(stack_.size() - argument_count - 1);
      stack_.push(result);
    }
  }
}

template <>
inline void Vm::handle(const instruction::Closure& closure_instr) {
  ENSURES(constants != nullptr);
  ENSURES(closure_instr.operand() < constants->size());
  auto value = (*constants)[closure_instr.operand()];
  auto func = value.as_object()->as<Function>();
  auto closure = heap_.make_object<Closure>(func);
  stack_.push(closure);
  const auto upvalues = closure_instr.upvalues();
  for (size_t i = 0; i < func->upvalue_count; ++i) {
    auto is_local = upvalues[i * 2];
    auto index = upvalues[i * 2 + 1];
    if (is_local) {
      auto value = &stack_[current_frame().start_of_stack + index];
      closure->upvalues[i] = heap_.make_upvalue(value);
    } else {
      closure->upvalues[i] = current_frame().closure->upvalues[index];
    }
  }
  ip += func->upvalue_count * 2;
}

template <>
inline void Vm::handle(const instruction::Close_upvalue&) {
  close_upvalues(&stack_.back());
  stack_.pop();
}

template <>
inline void Vm::handle(const instruction::Return&) {
  auto result = stack_.pop();
  auto stack_size = current_frame().start_of_stack;
  close_upvalues(&stack_[current_frame().start_of_stack]);
  call_frames_.pop();
  if (call_frames_.empty()) {
    stack_.pop();
  } else {
    stack_.resize(stack_size);
    stack_.push(result);
    const auto& chunk = current_frame().closure->func()->chunk();
    code = &chunk.code();
    constants = &chunk.constants();
    ip = current_frame().ip;
  }
}

#define INTERPRET_CASE(instr_struct, base)                \
  case instruction::instr_struct::opcode: {               \
    auto instr = instruction::instr_struct{&(*code)[ip]}; \
    ip += instr.size;                                     \
    handle(instr);                                        \
    break;                                                \
  }

template <bool Debug>
inline void Vm::interpret(Function* func) noexcept {
  try {
    auto closure = heap_.make_object<Closure>(func);
    stack_.push(closure);
    call_closure(closure, 0);
    while (ip < code->size() && !call_frames_.empty()) {
      switch ((*code)[ip]) { INSTRUCTIONS(INTERPRET_CASE) }
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
      auto& frame = call_frames_.peek(distance);
      out_ << "[line " << std::setfill('0') << std::setw(4)
           << frame.closure->func()->chunk().lines()[frame.ip] << " in] "
           << call_frames_.peek(distance).closure->func()->to_string() << "\n";
    }
  }
}

}  // namespace lox

#endif

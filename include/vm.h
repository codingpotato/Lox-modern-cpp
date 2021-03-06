#ifndef LOX_VM_H
#define LOX_VM_H

#include <ostream>
#include <string>

#include "compiler.h"
#include "exception.h"
#include "gc.h"
#include "hash_table.h"
#include "heap.h"
#include "instruction.h"
#include "native.h"
#include "object.h"
#include "scanner.h"
#include "stack.h"
#include "value.h"

namespace lox {

class VM {
 public:
  explicit VM(std::ostream& os) noexcept
      : out{&os},
        compiler{heap},
        gc{heap, globals, stack, call_frames, compiler} {
    register_natives(globals, heap);
  }

  template <bool Debug = false>
  inline void interpret(std::string source) noexcept;

  template <typename Instruction>
  void handle(const Instruction&) {
    EXPECTS(false);
  }

 private:
  struct Call_frame {
    Call_frame() noexcept {}
    Call_frame(Closure& closure, size_t bottom = 0) noexcept
        : closure{&closure},
          ip{closure.get_func()->get_chunk().code_begin()},
          bottom_of_stack{bottom} {}

    Closure* closure = nullptr;
    const Bytecode* ip;
    size_t bottom_of_stack = 0;
  };

  struct Executor {
    void copy_from(Closure& closure) noexcept {
      Chunk& chunk = closure.get_func()->get_chunk();
      ip = chunk.code_begin();
      end = chunk.code_end();
      constants = &chunk.get_constants();
    }

    void copy_from(const Call_frame& frame) noexcept {
      copy_from(*frame.closure);
      ip = frame.ip;
    }

    Value& constant_at(size_t pos) const noexcept {
      ENSURES(constants);
      ENSURES(pos < constants->size());
      return (*constants)[pos];
    }

    const Bytecode* ip = nullptr;
    const Bytecode* end = nullptr;
    Value_vector* constants = nullptr;
  };

  template <typename Func>
  void binary(Func func) {
    const auto right = stack.pop();
    const auto left = stack.pop();
    if (left.is_double() && right.is_double()) {
      stack.push(func(left, right));
    } else {
      throw_runtime_error("Operands must be numbers.");
    }
  }

  Call_frame& top_frame() noexcept {
    ENSURES(!call_frames.empty());
    return call_frames.peek();
  }

  void close_upvalues(const Value* last) noexcept {
    const auto& open_upvalues = heap.get_open_upvalues();
    for (auto it = open_upvalues.begin(); it != open_upvalues.end(); ++it) {
      if (it->location >= last) {
        it->closed = *it->location;
        it->location = &it->closed;
      }
    }
  }

  void call_closure(Closure& closure, size_t argument_count) {
    if (!call_frames.empty()) {
      top_frame().ip = executor.ip;
    }
    if (call_frames.size() < max_frame_size) {
      call_frames.push(closure, stack.size() - argument_count - 1);
      executor.copy_from(closure);
    } else {
      throw_runtime_error("Stack overflow.");
    }
  }

  bool concat_string(Value left, Value right) noexcept;

  void throw_runtime_error(const char* message);
  void throw_undefined_variable(const String* name);
  void throw_incorrect_argument_count(int arity, int argument_count);

  void backtrace() const noexcept;

  constexpr static size_t max_frame_size = 64;
  constexpr static size_t max_stacksize = max_frame_size * 256;

  using Value_stack = Stack<Value, max_stacksize>;
  using Call_frame_stack = Stack<Call_frame, max_frame_size>;

  std::ostream* out;
  Heap heap;
  Hash_table globals;
  Value_stack stack;
  Call_frame_stack call_frames;
  Compiler compiler;
  GC<Heap, Hash_table, Value_stack, Call_frame_stack, Compiler> gc;
  Executor executor;
};

template <>
inline void VM::handle(const instruction::Constant& constant) {
  stack.push(executor.constant_at(constant.operand()));
}

template <>
inline void VM::handle(const instruction::Nil&) {
  stack.push();
}

template <>
inline void VM::handle(const instruction::False&) {
  stack.push(false);
}

template <>
inline void VM::handle(const instruction::True&) {
  stack.push(true);
}

template <>
inline void VM::handle(const instruction::Pop&) {
  stack.pop();
}

template <>
inline void VM::handle(const instruction::Get_local& get_local) {
  stack.push(stack[top_frame().bottom_of_stack + get_local.operand()]);
}

template <>
inline void VM::handle(const instruction::Set_local& set_local) {
  stack[top_frame().bottom_of_stack + set_local.operand()] = stack.peek();
}

template <>
inline void VM::handle(const instruction::Get_global& get_global) {
  const auto constant = executor.constant_at(get_global.operand());
  const auto name = constant.as_object()->as<String>();
  if (const auto* value = globals.get_if(name); value != nullptr) {
    stack.push(*value);
  } else {
    throw_undefined_variable(name);
  }
}

template <>
inline void VM::handle(const instruction::Define_global& define_global) {
  const auto constant = executor.constant_at(define_global.operand());
  const auto name = constant.as_object()->as<String>();
  auto* str = heap.make_string(name->get_string());
  globals.insert(str, stack.pop());
}

template <>
inline void VM::handle(const instruction::Set_global& set_global) {
  const auto constant = executor.constant_at(set_global.operand());
  const auto name = constant.as_object()->as<String>();
  if (!globals.set(name, stack.peek())) {
    throw_undefined_variable(name);
  }
}

template <>
inline void VM::handle(const instruction::Get_upvalue& get_upvalue) {
  const auto slot = get_upvalue.operand();
  ENSURES(slot < top_frame().closure->get_upvalues().size());
  stack.push(*top_frame().closure->get_upvalues()[slot]->location);
}

template <>
inline void VM::handle(const instruction::Set_upvalue& set_upvalue) {
  auto slot = set_upvalue.operand();
  ENSURES(slot < top_frame().closure->get_upvalues().size());
  *top_frame().closure->get_upvalues()[slot]->location = stack.peek();
}

template <>
inline void VM::handle(const instruction::Equal&) {
  const auto right = stack.pop();
  const auto left = stack.pop();
  stack.push(left == right);
}

template <>
inline void VM::handle(const instruction::Greater&) {
  binary([](Value left, Value right) { return left > right; });
}

template <>
inline void VM::handle(const instruction::Less&) {
  binary([](Value left, Value right) { return left < right; });
}

template <>
inline void VM::handle(const instruction::Add&) {
  const auto right = stack.pop();
  const auto left = stack.pop();
  if (left.is_double() && right.is_double()) {
    stack.push(left + right);
  } else if (!concat_string(left, right)) {
    throw_runtime_error("Operands must be two numbers or two strings.");
  }
}

template <>
inline void VM::handle(const instruction::Subtract&) {
  binary([](Value left, Value right) { return left - right; });
}

template <>
inline void VM::handle(const instruction::Multiply&) {
  binary([](Value left, Value right) { return left * right; });
}

template <>
inline void VM::handle(const instruction::Divide&) {
  binary([](Value left, Value right) { return left / right; });
}

template <>
inline void VM::handle(const instruction::Not&) {
  stack.push(is_falsey(stack.pop()));
}

template <>
inline void VM::handle(const instruction::Negate&) {
  if (stack.peek().is_double()) {
    stack.push(-stack.pop().as_double());
  } else {
    throw_runtime_error("Operand must be a number.");
  }
}

template <>
inline void VM::handle(const instruction::Print&) {
  ENSURES(!stack.empty());
  *out << to_string(stack.pop()) << "\n";
}

template <>
inline void VM::handle(const instruction::Jump& jump) {
  executor.ip += jump.operand();
}

template <>
inline void VM::handle(const instruction::Jump_if_false& jump_if_false) {
  ENSURES(!stack.empty());
  if (is_falsey(stack.peek())) {
    executor.ip += jump_if_false.operand();
  }
}

template <>
inline void VM::handle(const instruction::Loop& loop) {
  executor.ip -= loop.operand();
}

template <>
inline void VM::handle(const instruction::Call& call) {
  const auto argument_count = call.operand();
  if (auto value = stack.peek(argument_count); value.is_object()) {
    auto object = value.as_object();
    if (object->is<Closure>()) {
      auto closure = object->as<Closure>();
      const auto arity = closure->get_func()->get_arity();
      if (argument_count == arity) {
        call_closure(*closure, argument_count);
        return;
      }
      throw_incorrect_argument_count(arity, argument_count);
    } else if (object->is<Native_func>()) {
      const auto func = object->as<Native_func>();
      const auto result = (*func)(
          argument_count,
          argument_count > 0 ? &stack[stack.size() - argument_count] : nullptr);
      stack.resize(stack.size() - argument_count - 1);
      stack.push(result);
      return;
    }
  }
  throw_runtime_error("Can only call functions and classes.");
}

template <>
inline void VM::handle(const instruction::Closure& closure_instr) {
  auto value = executor.constant_at(closure_instr.operand());
  auto* func = value.as_object()->as<Function>();
  auto* closure = heap.make_object<Closure>(func);
  stack.push(closure);
  const auto upvalues = closure_instr.upvalues();
  for (size_t i = 0; i < func->upvalue_count; ++i) {
    auto is_local = upvalues[i * 2];
    auto index = upvalues[i * 2 + 1];
    if (is_local) {
      auto value = &stack[top_frame().bottom_of_stack + index];
      closure->get_upvalues()[i] = heap.make_upvalue(value);
    } else {
      closure->get_upvalues()[i] = top_frame().closure->get_upvalues()[index];
    }
  }
  executor.ip += func->upvalue_count * 2;
}

template <>
inline void VM::handle(const instruction::Close_upvalue&) {
  close_upvalues(&stack.peek());
  stack.pop();
}

template <>
inline void VM::handle(const instruction::Return&) {
  auto result = stack.pop();
  auto stacksize = top_frame().bottom_of_stack;
  close_upvalues(&stack[top_frame().bottom_of_stack]);
  call_frames.pop();
  if (call_frames.empty()) {
    stack.pop();
  } else {
    stack.resize(stacksize);
    stack.push(result);
    executor.copy_from(top_frame());
  }
}

#define INTERPRET_CASE(instr_struct, base)               \
  case instruction::instr_struct::opcode: {              \
    auto instr = instruction::instr_struct{executor.ip}; \
    executor.ip += instr.size;                           \
    handle(instr);                                       \
    break;                                               \
  }

template <bool Debug>
inline void VM::interpret(std::string source) noexcept {
  try {
    lox::Scanner scanner{std::move(source)};
    auto func = compiler.compile(scanner.scan());
    auto closure = heap.make_object<Closure>(func);
    stack.push(closure);
    call_closure(*closure, 0);
    while (executor.ip != executor.end) {
      switch (*executor.ip) { INSTRUCTIONS(INTERPRET_CASE) }
      if constexpr (Debug) {
        for (size_t i = 0; i < stack.size(); ++i) {
          *out << to_string(stack[i]) << " ";
        }
        *out << "\n";
      }
    }
  } catch (Runtime_error& error) {
    *out << error.what() << "\n";
    backtrace();
  } catch (Exception& e) {
    *out << e.what() << "\n";
  }
}

}  // namespace lox

#endif

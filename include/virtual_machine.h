#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "chunk.h"
#include "exception.h"
#include "instruction.h"
#include "value.h"

#define DEBUG_TRACE_EXECUTION

namespace lox {

struct virtual_machine {
  inline void interpret(chunk ch);

  template <typename Opcode>
  void handle(oprand_t) {
    throw internal_error{"Need implement for " + Opcode::name};
  }

 private:
  template <typename... Args>
  void push(Args&&... args) noexcept {
    stack_.emplace_back(std::forward<Args>(args)...);
  }
  value pop() noexcept {
    auto v = stack_.back();
    stack_.pop_back();
    return v;
  }
  const value& peek() const noexcept { return stack_.back(); }

  chunk main_;
  std::vector<value> stack_;
  std::map<std::string, value> globals;
};

template <>
inline void virtual_machine::handle<op_constant>(oprand_t oprand) {
  push(main_.constants()[oprand]);
}

template <>
inline void virtual_machine::handle<op_nil>(oprand_t) {
  push();
}

template <>
inline void virtual_machine::handle<op_false>(oprand_t) {
  push(false);
}

template <>
inline void virtual_machine::handle<op_true>(oprand_t) {
  push(true);
}

template <>
inline void virtual_machine::handle<op_pop>(oprand_t) {
  pop();
}

template <>
inline void virtual_machine::handle<op_define_global>(oprand_t oprand) {
  const auto& name = main_.constants()[oprand];
  globals.emplace(name.as<std::string>(), std::move(pop()));
}

template <>
inline void virtual_machine::handle<op_equal>(oprand_t) {
  auto b = pop();
  auto a = pop();
  push(a == b);
}

template <>
inline void virtual_machine::handle<op_greater>(oprand_t) {
  auto b = pop();
  auto a = pop();
  push(a > b);
}

template <>
inline void virtual_machine::handle<op_less>(oprand_t) {
  auto b = pop();
  auto a = pop();
  push(a < b);
}

template <>
inline void virtual_machine::handle<op_add>(oprand_t) {
  auto b = pop();
  auto a = pop();
  push(a + b);
}

template <>
inline void virtual_machine::handle<op_subtract>(oprand_t) {
  auto b = pop();
  auto a = pop();
  push(a - b);
}

template <>
inline void virtual_machine::handle<op_multiply>(oprand_t) {
  auto b = pop();
  auto a = pop();
  push(a * b);
}

template <>
inline void virtual_machine::handle<op_divide>(oprand_t) {
  auto b = pop();
  auto a = pop();
  push(a / b);
}

template <>
inline void virtual_machine::handle<op_not>(oprand_t) {
  if (peek().is<bool>()) {
    push(!pop().as<bool>());
  } else {
    throw runtime_error{"Operand must be a boolean."};
  }
}

template <>
inline void virtual_machine::handle<op_negate>(oprand_t) {
  if (peek().is<double>()) {
    const auto operand = pop();
    const auto result = operand.is<nil>() || !operand.as<bool>();
    push(result);
  } else {
    throw runtime_error{"Operand must be a number."};
  }
}

template <>
inline void virtual_machine::handle<op_print>(oprand_t) {
  ENSURES(stack_.size() > 0);
  std::cout << pop().repr() << "\n";
}

template <>
inline void virtual_machine::handle<op_return>(oprand_t) {}

inline void virtual_machine::interpret(chunk ch) {
  main_ = std::move(ch);
  stack_.clear();
  for (auto& instr : main_.code()) {
#ifdef DEBUG_TRACE_EXECUTION
    std::cout << instr.visit([](auto opcode, auto) { return opcode.name; });
    std::cout << "    ";
    for (auto& v : stack_) {
      std::cout << "[ " << v.repr() << " ]";
    }
    std::cout << "\n";
#endif

    instr.visit(
        [this](auto opcode, auto oprand) { handle<decltype(opcode)>(oprand); });
  }
}

}  // namespace lox

#endif

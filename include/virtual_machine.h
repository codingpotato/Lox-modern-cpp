#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "chunk.h"
#include "exception.h"
#include "instruction.h"
#include "value.h"

#define DEBUG_TRACE_EXECUTION

namespace lox {

class virtual_machine {
 public:
  explicit virtual_machine(std::ostream& os) noexcept : out{os} {}

  inline void interpret(chunk ch);

  template <typename Opcode>
  void handle(oprand_t) {
    throw internal_error{"Need implement for " + Opcode::name};
  }

 private:
  template <typename Func>
  void binary(Func&& func) noexcept {
    auto right = pop();
    auto left = pop();
    push(std::forward<Func>(func)(left, right));
  }

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

  using value_vector = std::vector<value>;

  std::ostream& out;
  chunk main_;
  value_vector stack_;
  std::map<std::string, value> globals_;
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
inline void virtual_machine::handle<op_get_local>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  ENSURES(oprand < stack_.size());
  push(stack_[oprand]);
}

template <>
inline void virtual_machine::handle<op_set_local>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  const auto slot = main_.constants()[oprand].as<double>();
  ENSURES(slot < stack_.size());
  stack_[slot] = peek();
}

template <>
inline void virtual_machine::handle<op_get_global>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  const auto& name = main_.constants()[oprand].as<std::string>();
  if (globals_.find(name) != globals_.cend()) {
    push(globals_[name]);
  } else {
    throw runtime_error{"Undefined variable: " + name};
  }
}

template <>
inline void virtual_machine::handle<op_define_global>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  const auto& name = main_.constants()[oprand].as<std::string>();
  globals_.emplace(name, pop());
}

template <>
inline void virtual_machine::handle<op_set_global>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  const auto& name = main_.constants()[oprand].as<std::string>();
  if (globals_.find(name) != globals_.cend()) {
    globals_[name] = peek();
  } else {
    throw runtime_error{"Undefined variable: " + name};
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
  out << pop().repr() << "\n";
}

template <>
inline void virtual_machine::handle<op_return>(oprand_t) {}

inline void virtual_machine::interpret(chunk ch) {
  main_ = std::move(ch);
  stack_.clear();
  for (auto& instr : main_.code()) {
    instr.visit(
        [this](auto opcode, auto oprand) { handle<decltype(opcode)>(oprand); });
    ;
  }
}

}  // namespace lox

#endif

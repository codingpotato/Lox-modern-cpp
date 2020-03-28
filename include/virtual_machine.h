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
#include "value.h"

namespace lox {

class virtual_machine {
 public:
  explicit virtual_machine(std::ostream& os) noexcept : out{os} {}

  inline void interpret();

  const chunk& main() const noexcept { return main_; }

  template <typename Opcode>
  size_t add_instruction(int line, Opcode opcode,
                         oprand_t oprand = 0) noexcept {
    return main_.add_instruction(line, opcode, oprand);
  }

  std::size_t code_size() const noexcept { return main_.code().size(); }

  std::size_t add_constant_number(double number) noexcept {
    return main_.add_constant(number);
  }

  std::size_t add_constant_string(std::string str) noexcept {
    auto obj = heap_.add_string(std::move(str));
    return main_.add_constant(obj);
  }

  void set_oprand(std::size_t offset, oprand_t oprand) noexcept {
    return main_.set_oprand(offset, oprand);
  }

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
    stack_[count_++] = value{std::forward<Args>(args)...};
  }
  value pop() noexcept { return stack_[--count_]; }
  const value& peek() const noexcept { return stack_[count_ - 1]; }

 private:
  using value_vector = std::vector<value>;

  std::ostream& out;
  chunk main_;
  std::size_t ip_;
  value_vector stack_;
  size_t count_;
  heap heap_;
  hash_table globals_;
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
  ENSURES(oprand < stack_.size());
  push(stack_[oprand]);
}

template <>
inline void virtual_machine::handle<op_set_local>(oprand_t oprand) {
  ENSURES(oprand < stack_.size());
  stack_[oprand] = peek();
}

template <>
inline void virtual_machine::handle<op_get_global>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  const auto name = main_.constants()[oprand].as<object*>()->as<string>();
  if (globals_.contains(name)) {
    push(globals_[name]);
  } else {
    throw runtime_error{"Undefined variable: " +
                        static_cast<std::string>(*name)};
  }
}

template <>
inline void virtual_machine::handle<op_define_global>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  const auto name = main_.constants()[oprand].as<object*>()->as<string>();
  auto str = heap_.add_string(static_cast<std::string>(*name));
  globals_.insert(str, pop());
}

template <>
inline void virtual_machine::handle<op_set_global>(oprand_t oprand) {
  ENSURES(oprand < main_.constants().size());
  const auto name = main_.constants()[oprand].as<object*>()->as<string>();
  if (globals_.contains(name)) {
    globals_[name] = peek();
  } else {
    throw runtime_error{"Undefined variable: " +
                        static_cast<std::string>(*name)};
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
  ENSURES(!stack_.empty());
  out << pop().repr() << "\n";
}

template <>
inline void virtual_machine::handle<op_jump>(oprand_t oprand) {
  ip_ += oprand;
}

template <>
inline void virtual_machine::handle<op_jump_if_false>(oprand_t oprand) {
  ENSURES(!stack_.empty());
  if (!peek().as<bool>()) {
    ip_ += oprand;
  }
}

template <>
inline void virtual_machine::handle<op_loop>(oprand_t oprand) {
  ip_ -= oprand;
}

template <>
inline void virtual_machine::handle<op_return>(oprand_t) {}

#define SWITCH_CASE_(opcode, has_oprand_value) \
  case opcode::id:                             \
    handle<opcode>(instr.oprand());            \
    break;

inline void virtual_machine::interpret() {
  stack_.resize(10000);
  count_ = 0;
  ip_ = 0;
  while (ip_ < main_.code().size()) {
    const auto& instr = main_.code()[ip_++];
    switch (instr.raw_opcode()) { OPCODES(SWITCH_CASE_) };
  }
}

}  // namespace lox

#endif

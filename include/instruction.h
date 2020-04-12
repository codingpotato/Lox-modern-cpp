#ifndef LOX_INSTRUCTION_H
#define LOX_INSTRUCTION_H

#include <vector>

#include "contract.h"
#include "type_list.h"

namespace lox {

using Bytecode = uint8_t;
using Bytecode_vector = std::vector<Bytecode>;

namespace instruction {

struct Base {
  Base(const Bytecode_vector& code, size_t pos) noexcept
      : code_{code}, pos_{pos} {}

 protected:
  const Bytecode_vector& code_;
  size_t pos_;
};

struct Simple_instruction : Base {
  using Base::Base;

  static constexpr size_t size = sizeof(Bytecode);
};

struct Byte_instruction : Base {
  using Base::Base;

  static size_t add_operand(Bytecode_vector& code, size_t operand) noexcept {
    ENSURES(operand < UINT8_MAX);
    code.push_back(operand);
    return size - sizeof(Bytecode);
  }

  size_t operand() const noexcept { return code_[pos_ + 1]; }

  static constexpr size_t size = sizeof(Bytecode) * 2;
};

struct Constant_instruction : Byte_instruction {
  using Byte_instruction::Byte_instruction;
};

struct Short_instruction : Base {
  using Base::Base;

  static size_t add_operand(Bytecode_vector& code, size_t operand) noexcept {
    ENSURES(operand < UINT16_MAX);
    code.push_back(operand >> 8);
    code.push_back(operand & 0xff);
    return size - sizeof(Bytecode);
  }

  static size_t set_operand(Bytecode_vector& code, size_t pos,
                            size_t operand) noexcept {
    ENSURES(operand < UINT16_MAX);
    code[pos + 1] = operand >> 8;
    code[pos + 2] = operand & 0xff;
    return size - sizeof(Bytecode);
  }

  size_t operand() const noexcept {
    return (code_[pos_ + 1] << 8) | code_[pos_ + 2];
  }

  static constexpr size_t size = sizeof(Bytecode) + sizeof(Bytecode) * 2;
};

// clang-format off
#define INSTRUCTIONS(generator)                     \
  generator(Constant, Constant_instruction)         \
  generator(Nil, Simple_instruction)                \
  generator(True, Simple_instruction)               \
  generator(False, Simple_instruction)              \
  generator(Pop, Simple_instruction)                \
  generator(Get_local, Byte_instruction)            \
  generator(Set_local, Byte_instruction)            \
  generator(Get_global, Constant_instruction)       \
  generator(Define_global, Constant_instruction)    \
  generator(Set_global, Constant_instruction)       \
  generator(Get_upvalue, Byte_instruction)          \
  generator(Set_upvalue, Byte_instruction)          \
  generator(Equal, Simple_instruction)              \
  generator(Greater, Simple_instruction)            \
  generator(Less, Simple_instruction)               \
  generator(Add, Simple_instruction)                \
  generator(Subtract, Simple_instruction)           \
  generator(Multiply, Simple_instruction)           \
  generator(Divide, Simple_instruction)             \
  generator(Not, Simple_instruction)                \
  generator(Nagate, Simple_instruction)             \
  generator(Print, Simple_instruction)              \
  generator(Jump, Short_instruction)                \
  generator(Jump_if_false, Short_instruction)       \
  generator(Loop, Short_instruction)                \
  generator(Call, Byte_instruction)                 \
  generator(Closure, Constant_instruction)          \
  generator(Return, Simple_instruction)
// clang-format on

#define FORWARD_DECLARATION(instr, base) struct instr;

INSTRUCTIONS(FORWARD_DECLARATION)

#define TYPE_LIST_ARGUMENT(instr, base) instr,

using Types = Type_list<INSTRUCTIONS(TYPE_LIST_ARGUMENT) void>;
static_assert(Types::size < UINT8_MAX);

#define STRUCT(instr, base)                                         \
  struct instr : base {                                             \
    instr(const Bytecode_vector& code, size_t pos) noexcept         \
        : base{code, pos} {                                         \
      ENSURES(code[pos] == instr::opcode);                          \
    }                                                               \
                                                                    \
    static constexpr size_t opcode = Index_of<instr, Types>::value; \
    static constexpr const char* name = "OP_" #instr;               \
  };

INSTRUCTIONS(STRUCT)

#define VISIT_CASE(instr, base) \
  case instr::opcode:           \
    return std::forward<Visitor>(visitor)(instr{code, pos});

template <typename Visitor>
void visit(const Bytecode_vector& code, size_t pos, Visitor&& visitor) {
  switch (code[pos]) { INSTRUCTIONS(VISIT_CASE) }
}

}  // namespace instruction

}  // namespace lox

#endif

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

struct Simple : Base {
  using Base::Base;

  static constexpr size_t size = sizeof(Bytecode);
};

struct Byte : Base {
  using Base::Base;

  static size_t add_operand(Bytecode_vector& code, size_t operand) noexcept {
    ENSURES(operand < UINT8_MAX);
    code.push_back(operand);
    return size - sizeof(Bytecode);
  }

  size_t operand() const noexcept { return code_[pos_ + 1]; }

  static constexpr size_t size = sizeof(Bytecode) * 2;
};

struct Short : Base {
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
#define INSTRUCTIONS(generator)           \
  generator(Constant, Byte)               \
  generator(Nil, Simple)                  \
  generator(True, Simple)                 \
  generator(False, Simple)                \
  generator(Pop, Simple)                  \
  generator(Get_local, Byte)              \
  generator(Set_local, Byte)              \
  generator(Get_global, Byte)             \
  generator(Define_global, Byte)          \
  generator(Set_global, Byte)             \
  generator(Get_upvalue, Byte)            \
  generator(Set_upvalue, Byte)            \
  generator(Equal, Simple)                \
  generator(Greater, Simple)              \
  generator(Less, Simple)                 \
  generator(Add, Simple)                  \
  generator(Subtract, Simple)             \
  generator(Multiply, Simple)             \
  generator(Divide, Simple)               \
  generator(Not, Simple)                  \
  generator(Nagate, Simple)               \
  generator(Print, Simple)                \
  generator(Jump, Short)                  \
  generator(Jump_if_false, Short)         \
  generator(Loop, Short)                  \
  generator(Call, Byte)                   \
  generator(Closure, Byte)                \
  generator(Return, Simple)
// clang-format on

#define FORWARD_DECLARATION(instruction, base) struct instruction;

INSTRUCTIONS(FORWARD_DECLARATION)

#define TYPE_LIST_ARGUMENT(instruction, base) instruction,

using Types = Type_list<INSTRUCTIONS(TYPE_LIST_ARGUMENT) void>;
static_assert(Types::size < UINT8_MAX);

#define STRUCT(instruction, base)                                         \
  struct instruction : base {                                             \
    instruction(const Bytecode_vector& code, size_t pos) noexcept         \
        : base{code, pos} {                                               \
      ENSURES(code[pos] == instruction::opcode);                          \
    }                                                                     \
                                                                          \
    static constexpr size_t opcode = Index_of<instruction, Types>::value; \
    static constexpr const char* name = "OP_" #instruction;               \
  };

INSTRUCTIONS(STRUCT)

#define VISIT_CASE(instruction, base) \
  case instruction::opcode:           \
    return std::forward<Visitor>(visitor)(instruction{code, pos});

template <typename Visitor>
void visit(const Bytecode_vector& code, size_t pos, Visitor&& visitor) {
  switch (code[pos]) { INSTRUCTIONS(VISIT_CASE) }
}

}  // namespace instruction

}  // namespace lox

#endif

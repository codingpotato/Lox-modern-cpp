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
  Base(const Bytecode* code) noexcept : code_{code} {}

 protected:
  const Bytecode* code_;
};

struct Simple_instruction : Base {
  using Base::Base;
  static constexpr size_t size = sizeof(Bytecode);
};

struct Byte_instruction : Base {
  using Base::Base;

  static constexpr size_t index_operad = 1;

  static size_t add_operand(Bytecode_vector& code, size_t operand) noexcept {
    ENSURES(operand <= UINT8_MAX);
    code.push_back(operand);
    return size - sizeof(Bytecode);
  }
  size_t operand() const noexcept { return code_[index_operad]; }

  static constexpr size_t size = sizeof(Bytecode) + sizeof(Bytecode);
};

struct Constant_instruction : Byte_instruction {
  using Byte_instruction::Byte_instruction;
};

struct Jump_instruction : Base {
  using Base::Base;

  static constexpr size_t index_operad_high_byte = 1;
  static constexpr size_t index_operand_low_byte = 2;

  static size_t add_operand(Bytecode_vector& code, size_t operand) noexcept {
    ENSURES(operand <= UINT16_MAX);
    code.push_back(high_byte_of(operand));
    code.push_back(low_byte_of(operand));
    return size - sizeof(Bytecode);
  }

  static void set_operand(Bytecode* code, size_t operand) noexcept {
    ENSURES(operand < UINT16_MAX);
    code[index_operad_high_byte] = high_byte_of(operand);
    code[index_operand_low_byte] = low_byte_of(operand);
  }

  size_t operand() const noexcept {
    return (code_[index_operad_high_byte] << 8) | code_[index_operand_low_byte];
  }

  static constexpr size_t size = sizeof(Bytecode) + sizeof(Bytecode) * 2;

 private:
  static Bytecode high_byte_of(size_t operand) noexcept { return operand >> 8; }
  static Bytecode low_byte_of(size_t operand) noexcept {
    return operand & 0xff;
  }
};

struct Closure_instruction : Constant_instruction {
  struct Upvalue {
    Upvalue(size_t index, bool is_local) noexcept
        : index{static_cast<Bytecode>(index)},
          is_local{static_cast<Bytecode>(is_local ? 1 : 0)} {
      ENSURES(i < UINT8_MAX);
    }
    Bytecode index;
    Bytecode is_local;
  };
  using Upvalue_vector = std::vector<Upvalue>;

  using Constant_instruction::Constant_instruction;

  static constexpr size_t index_of_upvalues = 2;

  static size_t add_operand(Bytecode_vector& code, size_t operand,
                            const Upvalue_vector& upvalues) noexcept {
    const auto size = Constant_instruction::add_operand(code, operand);
    for (const auto& upvalue : upvalues) {
      code.push_back(upvalue.is_local);
      code.push_back(upvalue.index);
    }
    return size + upvalues.size() * 2;
  }

  const Bytecode* upvalues() const noexcept {
    return &code_[index_of_upvalues];
  }
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
  generator(Jump, Jump_instruction)                 \
  generator(Jump_if_false, Jump_instruction)        \
  generator(Loop, Jump_instruction)                 \
  generator(Call, Byte_instruction)                 \
  generator(Closure, Closure_instruction)           \
  generator(Close_upvalue, Simple_instruction)      \
  generator(Return, Simple_instruction)
// clang-format on

#define FORWARD_DECLARATION(instr, base) struct instr;

INSTRUCTIONS(FORWARD_DECLARATION)

#define TYPE_LIST_ARGUMENT(instr, base) instr,

using Types = Type_list<INSTRUCTIONS(TYPE_LIST_ARGUMENT) void>;
static_assert(Types::size < UINT8_MAX);

#define STRUCT(instr, base)                                         \
  struct instr : base {                                             \
    instr(const Bytecode* code) noexcept : base{code} {             \
      ENSURES(*code == instr::opcode);                              \
    }                                                               \
                                                                    \
    static constexpr size_t opcode = Index_of<instr, Types>::value; \
    static constexpr const char* name = "OP_" #instr;               \
  };

INSTRUCTIONS(STRUCT)

#define VISIT_CASE(instr, base) \
  case instr::opcode:           \
    return std::forward<Visitor>(visitor)(instr{&code[pos]});

template <typename Visitor>
void visit(const Bytecode_vector& code, size_t pos, Visitor&& visitor) {
  switch (code[pos]) { INSTRUCTIONS(VISIT_CASE) }
}

}  // namespace instruction

}  // namespace lox

#endif

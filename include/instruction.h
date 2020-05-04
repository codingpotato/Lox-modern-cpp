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
  Base(const Bytecode* code) noexcept : code{code} {}

  const Bytecode* get_code() const noexcept { return code; }

 protected:
  const Bytecode* code;
};

struct Simple_instr : Base {
  static constexpr size_t size = sizeof(Bytecode);

  using Base::Base;
};

struct Byte_instr : Base {
  using Operand_t = uint8_t;

  static constexpr size_t size = sizeof(Bytecode) + sizeof(Operand_t);
  static constexpr Operand_t operand_max = UINT8_MAX;
  static constexpr size_t operand_index = 1;

  static size_t add_operand(Bytecode_vector& code, Operand_t operand) noexcept {
    ENSURES(operand <= operand_max);
    code.push_back(operand);
    return sizeof(Operand_t);
  }

  using Base::Base;

  Operand_t operand() const noexcept { return code[operand_index]; }
};

struct Constant_instr : Byte_instr {
  using Byte_instr::Byte_instr;
};

struct Jump_instr : Base {
  using Operand_t = uint16_t;

  static constexpr size_t size = sizeof(Bytecode) + sizeof(Operand_t);
  static constexpr Operand_t operand_max = UINT16_MAX;
  static constexpr size_t operand_index_high = 1;
  static constexpr size_t operand_index_low = 2;

  static size_t add_operand(Bytecode_vector& code, Operand_t operand) noexcept {
    ENSURES(operand <= operand_max);
    code.push_back(high_byte_of(operand));
    code.push_back(low_byte_of(operand));
    return sizeof(Operand_t);
  }

  static void set_operand(Bytecode* code, Operand_t operand) noexcept {
    ENSURES(operand < UINT16_MAX);
    code[operand_index_high] = high_byte_of(operand);
    code[operand_index_low] = low_byte_of(operand);
  }

  using Base::Base;

  Operand_t operand() const noexcept {
    return (code[operand_index_high] << 8) | code[operand_index_low];
  }

 private:
  static Bytecode high_byte_of(size_t operand) noexcept { return operand >> 8; }
  static Bytecode low_byte_of(size_t operand) noexcept {
    return operand & 0xff;
  }
};

struct Closure_instr : Constant_instr {
  struct Upvalue {
    Upvalue(size_t index, bool is_local) noexcept
        : index{static_cast<Bytecode>(index)},
          is_local{static_cast<Bytecode>(is_local ? 1 : 0)} {
      ENSURES(index <= UINT8_MAX);
    }
    Bytecode index;
    Bytecode is_local;
  };
  using Upvalue_vector = std::vector<Upvalue>;

  using Constant_instr::Constant_instr;

  static constexpr size_t index_of_upvalues = 2;

  static size_t add_upvalues(Bytecode_vector& code,
                             const Upvalue_vector& upvalues) noexcept {
    for (const auto& upvalue : upvalues) {
      code.push_back(upvalue.is_local);
      code.push_back(upvalue.index);
    }
    return upvalues.size() * 2;
  }

  const Bytecode* upvalues() const noexcept { return &code[index_of_upvalues]; }
};

// clang-format off
#define INSTRUCTIONS(generator)               \
  generator(Constant, Constant_instr)         \
  generator(Nil, Simple_instr)                \
  generator(True, Simple_instr)               \
  generator(False, Simple_instr)              \
  generator(Pop, Simple_instr)                \
  generator(Get_local, Byte_instr)            \
  generator(Set_local, Byte_instr)            \
  generator(Get_global, Constant_instr)       \
  generator(Define_global, Constant_instr)    \
  generator(Set_global, Constant_instr)       \
  generator(Get_upvalue, Byte_instr)          \
  generator(Set_upvalue, Byte_instr)          \
  generator(Equal, Simple_instr)              \
  generator(Greater, Simple_instr)            \
  generator(Less, Simple_instr)               \
  generator(Add, Simple_instr)                \
  generator(Subtract, Simple_instr)           \
  generator(Multiply, Simple_instr)           \
  generator(Divide, Simple_instr)             \
  generator(Not, Simple_instr)                \
  generator(Negate, Simple_instr)             \
  generator(Print, Simple_instr)              \
  generator(Jump, Jump_instr)                 \
  generator(Jump_if_false, Jump_instr)        \
  generator(Loop, Jump_instr)                 \
  generator(Call, Byte_instr)                 \
  generator(Closure, Closure_instr)           \
  generator(Close_upvalue, Simple_instr)      \
  generator(Return, Simple_instr)
// clang-format on

#define FORWARD_DECLARATION(instr, base) struct instr;

INSTRUCTIONS(FORWARD_DECLARATION)

#define TYPE_LIST_ARGUMENT(instr, base) instr,

using Types = Type_list<INSTRUCTIONS(TYPE_LIST_ARGUMENT) void>;
static_assert(Types::size < UINT8_MAX);

#define STRUCT(instr, base)                                           \
  struct instr : base {                                               \
    instr(const Bytecode* code) noexcept : base{code} {               \
      ENSURES(*code == instr::opcode);                                \
    }                                                                 \
                                                                      \
    static constexpr Bytecode opcode = Index_of<instr, Types>::value; \
    static constexpr const char* name = "OP_" #instr;                 \
  };

INSTRUCTIONS(STRUCT)

#define VISIT_CASE(instr, base) \
  case instr::opcode:           \
    return visitor(instr{&code[pos]});

template <typename Visitor>
void visit(const Bytecode_vector& code, size_t pos, Visitor&& visitor) {
  switch (code[pos]) { INSTRUCTIONS(VISIT_CASE) }
}

}  // namespace instruction

}  // namespace lox

#endif

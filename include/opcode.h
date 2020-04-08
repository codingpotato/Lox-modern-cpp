#ifndef LOX_OPCODE_H
#define LOX_OPCODE_H

#include "contract.h"
#include "type_list.h"

namespace lox {

using Byte_code = uint8_t;

struct Simple_instruction {};

struct Byte_instruction {};

struct Constant_instruction {};

struct Jump_instruction {};

#define OPCODES(generator)                            \
  generator(OP_constant, Constant_instruction)        \
      generator(OP_nil, Simple_instruction)           \
          generator(OP_true, Simple_instruction)      \
              generator(OP_false, Simple_instruction) \
                  generator(OP_pop, Simple_instruction)

#define FORWARD_DECLARATION(opcode, base) struct opcode;

OPCODES(FORWARD_DECLARATION)

#define TYPE_LIST_ARGUMENT(opcode, base) opcode,

using OP_types = Type_list<OPCODES(TYPE_LIST_ARGUMENT) void>;
static_assert(OP_types::size < UINT8_MAX);

#define STRUCT(opcode, base)                                        \
  struct opcode : base {                                            \
    static constexpr size_t id = Index_of<opcode, OP_types>::value; \
  };

OPCODES(STRUCT)

template <typename Opcode>
struct Instruction {
  explicit Instruction(const Byte_code* code) noexcept : code_{code} {
    ENSURE(*code == Opcode::id);
  }

 private:
  const Byte_code* code_;
};

}  // namespace lox

#endif

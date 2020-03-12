#ifndef LOX_INSTRUCTION_H
#define LOX_INSTRUCTION_H

#include <iomanip>
#include <sstream>
#include <string>

#include "contract.h"
#include "type_list.h"

#define OPCODES(generator)                                              \
  generator(op_constant, true) generator(op_nil, false)                 \
      generator(op_true, false) generator(op_false, false)              \
          generator(op_add, false) generator(op_subtract, false)        \
              generator(op_multiply, false) generator(op_divide, false) \
                  generator(op_not, false) generator(op_negate, false)  \
                      generator(op_return, false)

#define FORWARD_DECLARATION(opcode, has_oprand_value) struct opcode;

#define TYPE_LIST_ARGUMENT(opcode, has_oprand_value) opcode,

#define STRUCT(opcode, has_oprand_value)                              \
  struct opcode {                                                     \
    static constexpr std::size_t id = index_of<opcode, types>::value; \
    static constexpr const char* name = #opcode;                      \
    static constexpr bool has_oprand = has_oprand_value;              \
  };

#define SWITCH_CASE(opcode, has_oprand_value) \
  case opcode::id:                            \
    return std::forward<Visitor>(visitor)(opcode{}, oprand());

namespace lox {

OPCODES(FORWARD_DECLARATION)

using types = type_list<OPCODES(TYPE_LIST_ARGUMENT) void>;

OPCODES(STRUCT)

using oprand_t = unsigned int;

struct instruction {
  template <typename Opcode>
  constexpr instruction(Opcode, oprand_t oprand = 0) noexcept
      : raw_data_{raw_data_of<Opcode>(oprand)} {}

  template <typename Visitor>
  auto visit(Visitor&& visitor) const {
    switch (raw_opcode()) { OPCODES(SWITCH_CASE) }
    throw internal_error("Unknow opcode.");
  }

  constexpr oprand_t oprand() const noexcept { return raw_data_ & oprand_mask; }

 private:
  using raw_data_t = unsigned int;

  constexpr static unsigned int char_bits = 8;
  constexpr static unsigned int opcode_bits = 8;
  constexpr static unsigned int oprand_bits =
      sizeof(raw_data_t) * char_bits - opcode_bits;
  constexpr static unsigned int oprand_mask = 0xffff;

  constexpr std::size_t raw_opcode() const noexcept {
    return raw_data_ >> oprand_bits;
  }

  template <typename Opcode>
  static constexpr raw_data_t raw_data_of(oprand_t oprand) noexcept {
    ENSURES(oprand <= 0xffff);
    return Opcode::id << oprand_bits | oprand;
  }

  raw_data_t raw_data_;
};

}  // namespace lox

#endif
#ifndef LOX_INSTRUCTION_H
#define LOX_INSTRUCTION_H

#include <iomanip>
#include <sstream>
#include <string>

#include "contract.h"
#include "exception.h"

namespace lox {

struct instruction {
  enum opcode_t {
    op_constant = 0,
    op_add,
    op_subtract,
    op_multiply,
    op_divide,
    op_negate,
    op_return
  };
  using oprand_t = unsigned int;

  constexpr instruction(opcode_t opcode, oprand_t oprand = 0) noexcept
      : raw_data_{raw_data_from(opcode, oprand)} {}

  constexpr opcode_t opcode() const noexcept {
    return static_cast<opcode_t>(raw_data_ >> oprand_bits);
  }
  constexpr oprand_t oprand() const noexcept { return raw_data_ & oprand_mask; }

  template <typename Func>
  std::string repr(Func&& callback) const {
    switch (opcode()) {
      case op_constant:
        return "OP_CONSTANT " + callback(oprand());
      case op_add:
        return "OP_ADD";
      case op_subtract:
        return "OP_SUBTRACT";
      case op_multiply:
        return "OP_MULTIPLY";
      case op_divide:
        return "OP_DIVIDE";
      case op_negate:
        return "OP_NEGATE";
      case op_return:
        return "OP_RETURN";
      default:
        throw internal_error{"Uknown opcode."};
    }
  }

 private:
  using raw_data_t = unsigned int;
  constexpr static unsigned int oprand_bits = sizeof(raw_data_t) * 8 - 8;
  constexpr static unsigned int oprand_mask = 0xffff;

  static constexpr raw_data_t raw_data_from(opcode_t opcode,
                                            oprand_t oprand) noexcept {
    ENSURES(oprand <= 0xffff);
    return opcode << oprand_bits | oprand;
  }

  raw_data_t raw_data_;
};

}  // namespace lox

#endif

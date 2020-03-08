#ifndef LOX_INSTRUCTION_H
#define LOX_INSTRUCTION_H

#include <iomanip>
#include <sstream>
#include <string>

#include "contract.h"
#include "exception.h"

namespace lox {

struct instruction {
  enum opcode_t { op_constant = 0, op_return };
  using oprand_t = unsigned int;

  constexpr instruction(opcode_t opcode, oprand_t oprand = 0) noexcept
      : raw_data{raw_data_from(opcode, oprand)} {}

  template <typename Func>
  std::string repr(Func&& callback) const {
    switch (opcode()) {
      case op_constant:
        return "OP_CONSTANT " + callback(oprand()) + "\n";
      case op_return:
        return "OP_RETURN\n";
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

  constexpr opcode_t opcode() const noexcept {
    return static_cast<opcode_t>(raw_data >> oprand_bits);
  }
  constexpr oprand_t oprand() const noexcept { return raw_data & oprand_mask; }

  raw_data_t raw_data;
};

}  // namespace lox

#endif

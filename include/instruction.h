#ifndef LOX_INSTRUCTION_H
#define LOX_INSTRUCTION_H

#include <iomanip>
#include <sstream>
#include <string>

#include "contract.h"
#include "exception.h"

namespace lox {

struct instruction {
#define INSTRUCTIONS                                                      \
  GENERATOR(op_constant), GENERATOR(op_nil), GENERATOR(op_true),          \
      GENERATOR(op_false), GENERATOR(op_add), GENERATOR(op_subtract),     \
      GENERATOR(op_multiply), GENERATOR(op_divide), GENERATOR(op_negate), \
      GENERATOR(op_return),

#define GENERATOR(x) x

  enum opcode_t { INSTRUCTIONS };

  using oprand_t = unsigned int;

  constexpr instruction(opcode_t opcode, oprand_t oprand = 0) noexcept
      : raw_data_{raw_data_from(opcode, oprand)} {}

  constexpr opcode_t opcode() const noexcept {
    return static_cast<opcode_t>(raw_data_ >> oprand_bits);
  }
  constexpr oprand_t oprand() const noexcept { return raw_data_ & oprand_mask; }

  template <typename Func>
  std::string repr(Func&& callback) const noexcept {
    return std::string{instruction_names[opcode()]} + " " + callback(oprand());
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

#undef GENERATOR
#define GENERATOR(instruction) #instruction

  static constexpr const char* instruction_names[] = {INSTRUCTIONS};

  raw_data_t raw_data_;
};

}  // namespace lox

#endif

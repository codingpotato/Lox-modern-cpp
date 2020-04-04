#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <sstream>
#include <string>
#include <vector>

#include "instruction.h"
#include "value.h"

namespace lox {

class chunk {
 public:
  template <typename Opcode>
  size_t add_instruction(int line, Opcode opcode,
                         oprand_t oprand = 0) noexcept {
    instructions.emplace_back(instruction{opcode, oprand});
    lines.emplace_back(line);
    EXPECTS(lines.size() == instructions.size());
    return instructions.size() - 1;
  }

  template <typename... Args>
  size_t add_constant(Args&&... args) noexcept {
    constants.emplace_back(std::forward<Args>(args)...);
    return constants.size() - 1;
  }

  size_t instruction_size() const noexcept { return instructions.size(); }
  instruction instruction_at(size_t pos) const noexcept {
    return instructions[pos];
  }
  size_t constant_size() const noexcept { return constants.size(); }
  value constant_at(size_t pos) const noexcept { return constants[pos]; }

  void set_oprand(size_t offset, oprand_t oprand) noexcept {
    instructions[offset].set_oprand(oprand);
  }

  std::string repr(const std::string& name) const noexcept {
    std::string string = "== " + name + " ==\n";
    for (std::size_t offset = 0; offset < instructions.size(); ++offset) {
      std::ostringstream oss;
      oss << std::setfill('0') << std::setw(4) << offset << " ";
      if (offset == 0 || lines[offset] != lines[offset - 1]) {
        oss << std::setfill(' ') << std::setw(4) << lines[offset] << " ";
      } else {
        oss << "   | ";
      }
      string += oss.str();
      string += instructions[offset].visit([this](auto opcode, auto oprand) {
        std::ostringstream oss;
        oss << opcode.name;
        if (opcode.has_oprand) {
          ENSURES(oprand < constants.size());
          oss << " " << constants[static_cast<std::size_t>(oprand)].repr();
        }
        return oss.str();
      }) + "\n";
    }
    return string;
  }

 private:
  using instruction_vector = std::vector<instruction>;
  using value_vector = std::vector<value>;
  using line_vector = std::vector<int>;

  instruction_vector instructions;
  value_vector constants;
  line_vector lines;
};

}  // namespace lox

#endif

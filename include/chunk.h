#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <sstream>
#include <string>
#include <vector>

#include "instruction.h"
#include "value.h"

namespace lox {

struct chunk {
  void add_instruction(instruction::opcode_t opcode, int line) noexcept {
    code.emplace_back(opcode);
    lines.emplace_back(line);
    EXPECTS(lines.size() == code.size());
  }
  void add_instruction(instruction::opcode_t opcode,
                       instruction::oprand_t oprand, int line) noexcept {
    code.emplace_back(opcode, oprand);
    lines.emplace_back(line);
    EXPECTS(lines.size() == code.size());
  }

  template <typename... Args>
  std::size_t add_constant(Args&&... args) noexcept {
    constants.emplace_back(std::forward<Args>(args)...);
    return constants.size() - 1;
  }

  std::string repr(const std::string& name) const noexcept {
    std::string string = "== " + name + " ==\n";
    for (std::size_t offset = 0; offset < code.size(); ++offset) {
      std::ostringstream oss;
      oss << std::setfill('0') << std::setw(4) << offset << " ";
      if (offset == 0 || lines[offset] != lines[offset - 1]) {
        oss << std::setfill(' ') << std::setw(4) << lines[offset] << " ";
      } else {
        oss << "   | ";
      }
      string += oss.str();
      string += code[offset].repr([this](auto oprand) {
        ENSURES(oprand < constants.size());
        std::ostringstream oss;
        oss << constants[oprand];
        return oss.str();
      });
    }
    return string;
  }

 private:
  std::vector<instruction> code;
  std::vector<value> constants;
  std::vector<int> lines;
};

}  // namespace lox

#endif

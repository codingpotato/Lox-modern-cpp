#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <sstream>
#include <string>
#include <vector>

#include "instruction.h"
#include "value.h"

namespace lox {

struct chunk {
  using instruction_vector = std::vector<instruction>;
  using line_vector = std::vector<size_t>;
  using value_vector = std::vector<value>;

  const instruction_vector& instructions() const noexcept {
    return instructions_;
  }
  const line_vector& lines() const noexcept { return lines_; }
  const value_vector& constants() const noexcept { return constants_; }

  template <typename Opcode>
  size_t add_instruction(int line, Opcode opcode,
                         oprand_t oprand = 0) noexcept {
    instructions_.emplace_back(instruction{opcode, oprand});
    lines_.emplace_back(line);
    EXPECTS(lines_.size() == instructions_.size());
    return instructions_.size() - 1;
  }

  template <typename... Args>
  size_t add_constant(Args&&... args) noexcept {
    constants_.emplace_back(std::forward<Args>(args)...);
    return constants_.size() - 1;
  }

  void set_oprand(size_t pos, oprand_t oprand) noexcept {
    instructions_[pos].set_oprand(oprand);
  }

 private:
  instruction_vector instructions_;
  line_vector lines_;
  value_vector constants_;
};

inline std::string to_string(const chunk& code, const std::string& name,
                             int level = 0) noexcept {
  const auto& instructions = code.instructions();
  const auto& lines = code.lines();
  const auto& constants = code.constants();
  std::string result = level == 0 ? "== " + name + " ==\n" : name + "\n";
  for (std::size_t pos = 0; pos < instructions.size(); ++pos) {
    std::ostringstream oss;
    oss << std::string(level * 4, ' ') << std::setfill('0') << std::setw(4)
        << pos << " ";
    if (pos == 0 || lines[pos] != lines[pos - 1]) {
      oss << std::setfill(' ') << std::setw(4) << lines[pos] << " ";
    } else {
      oss << "   | ";
    }
    result += oss.str();
    result += instructions[pos].visit([&](auto opcode, auto oprand) {
      std::ostringstream oss;
      oss << opcode.name;
      if constexpr (std::is_same_v<decltype(opcode), op_constant>) {
        ENSURES(oprand < constants.size());
        oss << " " << to_string(constants[oprand], true);
      } else if (opcode.has_oprand) {
        oss << " " << oprand;
      }
      return oss.str();
    }) + "\n";
  }
  return result;
}

}  // namespace lox

#endif

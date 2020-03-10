#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <sstream>
#include <string>
#include <vector>

#include "instruction.h"
#include "value.h"

namespace lox {

struct chunk {
  using code_vector = std::vector<instruction>;
  using value_vector = std::vector<value>;
  using line_vector = std::vector<int>;

  void add_instruction(instruction::opcode_t opcode, int line) noexcept {
    code_.emplace_back(opcode);
    lines_.emplace_back(line);
    EXPECTS(lines_.size() == code_.size());
  }
  void add_instruction(instruction::opcode_t opcode,
                       instruction::oprand_t oprand, int line) noexcept {
    code_.emplace_back(opcode, oprand);
    lines_.emplace_back(line);
    EXPECTS(lines_.size() == code_.size());
  }

  template <typename... Args>
  std::size_t add_constant(Args&&... args) noexcept {
    constants_.emplace_back(std::forward<Args>(args)...);
    return constants_.size() - 1;
  }

  const code_vector& code() const noexcept { return code_; }
  const value_vector& constants() const noexcept { return constants_; }

  std::string repr(const std::string& name) const noexcept {
    std::string string = "== " + name + " ==\n";
    for (std::size_t offset = 0; offset < code_.size(); ++offset) {
      std::ostringstream oss;
      oss << std::setfill('0') << std::setw(4) << offset << " ";
      if (offset == 0 || lines_[offset] != lines_[offset - 1]) {
        oss << std::setfill(' ') << std::setw(4) << lines_[offset] << " ";
      } else {
        oss << "   | ";
      }
      string += oss.str();
      string += code_[offset].repr([this](const auto& oprand) {
        ENSURES(oprand < constants_.size());
        std::ostringstream oss;
        oss << constants_[static_cast<std::size_t>(oprand)].as<double>();
        return oss.str();
      }) + "\n";
    }
    return string;
  }

 private:
  std::vector<instruction> code_;
  std::vector<value> constants_;
  std::vector<int> lines_;
};

}  // namespace lox

#endif

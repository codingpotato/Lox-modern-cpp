#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <iomanip>
#include <string>
#include <vector>

#include "instruction.h"
#include "value.h"

namespace lox {

using Line_vector = std::vector<size_t>;
using Value_vector = std::vector<Value>;

struct Chunk {
  const Bytecode_vector& code() const noexcept { return code_; }
  const Line_vector& lines() const noexcept { return lines_; }
  const Value_vector& constants() const noexcept { return constants_; }

  template <typename Instruction>
  size_t add(size_t line) noexcept {
    auto pos = code_.size();
    code_.push_back(Instruction::opcode);
    lines_.push_back(line);
    EXPECTS(code_.size() == lines_.size());
    return pos;
  }

  template <typename Instruction>
  size_t add(size_t operand, size_t line) noexcept {
    auto pos = code_.size();
    code_.push_back(Instruction::opcode);
    lines_.push_back(line);
    auto operand_count = Instruction::add_operand(code_, operand);
    for (size_t i = 0; i < operand_count; ++i) {
      lines_.push_back(line);
    }
    EXPECTS(code_.size() == lines_.size());
    return pos;
  }

  void add(Bytecode bytecode, size_t line) noexcept {
    code_.push_back(bytecode);
    lines_.push_back(line);
    EXPECTS(code_.size() == lines_.size());
  }

  template <typename... Args>
  size_t add_constant(Args&&... args) noexcept {
    constants_.emplace_back(std::forward<Args>(args)...);
    return constants_.size() - 1;
  }

  void paych_jump(size_t pos, size_t operand) noexcept {
    instruction::Short_instruction::set_operand(code_, pos, operand);
  }

 private:
  Bytecode_vector code_;
  Line_vector lines_;
  Value_vector constants_;
};

std::string upvalues_to_string(const Chunk& chunk, size_t pos,
                               const instruction::Closure& closure) noexcept;

template <typename Instruction>
inline std::string to_string(const Chunk& chunk, size_t pos,
                             const Instruction& instr) noexcept {
  std::ostringstream oss;
  oss << instr.name;
  if constexpr (std::is_base_of_v<instruction::Constant_instruction,
                                  Instruction>) {
    const auto& constants = chunk.constants();
    ENSURES(instr.operand() < constants.size());
    oss << " " << to_string(constants[instr.operand()], true);
  } else if constexpr (Instruction::size > sizeof(Bytecode)) {
    oss << " " << instr.operand();
    if constexpr (std::is_same_v<Instruction, instruction::Jump> ||
                  std::is_same_v<Instruction, instruction::Jump_if_false>) {
      oss << " -> " << pos + instruction::Jump::size + instr.operand();
    } else if constexpr (std::is_same_v<Instruction, instruction::Loop>) {
      oss << " -> " << pos + instruction::Loop::size - instr.operand();
    }
  }
  if constexpr (std::is_same_v<Instruction, instruction::Closure>) {
    oss << "        upvalues: " + upvalues_to_string(chunk, pos, instr);
  }
  return oss.str();
}

inline std::string to_string(const Chunk& chunk, const std::string& name,
                             int level = 0) noexcept {
  const auto& code = chunk.code();
  const auto& lines = chunk.lines();
  std::string result = level == 0 ? "== " + name + " ==\n" : name + "\n";
  size_t pos = 0;
  while (pos < code.size()) {
    std::ostringstream oss;
    oss << std::string(level * 4, ' ') << std::setfill('0') << std::setw(4)
        << pos << " ";
    if (pos == 0 || lines[pos] != lines[pos - 1]) {
      oss << std::setfill(' ') << std::setw(4) << lines[pos] << " ";
    } else {
      oss << "   | ";
    }
    result += oss.str();
    instruction::visit(code, pos, [&](const auto& instr) {
      result += to_string(chunk, pos, instr);
      pos += instr.size;
    });
    result += "\n";
  }
  return result;
}

}  // namespace lox

#endif

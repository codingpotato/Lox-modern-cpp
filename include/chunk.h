#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <iomanip>
#include <string>
#include <utility>
#include <vector>

#include "instruction.h"
#include "value.h"

namespace lox {

struct Chunk {
  using Line_vector = std::vector<size_t>;

  const Bytecode_vector& get_code() const noexcept { return code; }
  const Line_vector& get_lines() const noexcept { return lines; }
  const Value_vector& get_constants() const noexcept { return constants; }

  template <typename Instruction>
  size_t add(size_t line) noexcept {
    const auto pos = code.size();
    code.push_back(Instruction::opcode);
    lines.push_back(line);
    EXPECTS(code.size() == lines.size());
    return pos;
  }

  template <typename Instruction>
  size_t add(size_t operand, size_t line) noexcept {
    const auto pos = code.size();
    code.push_back(Instruction::opcode);
    lines.push_back(line);
    const auto bytecode_count = Instruction::add_operand(code, operand);
    for (size_t i = 0; i < bytecode_count; ++i) {
      lines.push_back(line);
    }
    EXPECTS(code.size() == lines.size());
    return pos;
  }

  template <typename Instruction>
  size_t add(size_t operand,
             const typename Instruction::Upvalue_vector& upvalues,
             size_t line) noexcept {
    const auto pos = code.size();
    code.push_back(Instruction::opcode);
    lines.push_back(line);
    const auto bytecode_count =
        Instruction::add_operand(code, operand, upvalues);
    for (size_t i = 0; i < bytecode_count; ++i) {
      lines.push_back(line);
    }
    EXPECTS(code.size() == lines.size());
    return pos;
  }

  template <typename... Args>
  size_t add_constant(Args&&... args) noexcept {
    constants.emplace_back(std::forward<Args>(args)...);
    return constants.size() - 1;
  }

  void patch_jump(size_t pos, size_t operand) noexcept {
    ENSURES(pos < code.size());
    instruction::Jump_instruction::set_operand(&code[pos], operand);
  }

 private:
  Bytecode_vector code;
  Line_vector lines;
  Value_vector constants;
};

std::pair<std::string, size_t> upvalues_to_string(
    const instruction::Closure& closure,
    const Value_vector& constants) noexcept;

template <typename Instruction>
inline std::pair<std::string, size_t> to_string(
    const Instruction& instr, size_t pos,
    const Value_vector& constants) noexcept {
  std::ostringstream oss;
  oss << instr.name;
  if constexpr (std::is_base_of_v<instruction::Constant_instruction,
                                  Instruction>) {
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
    auto [string, size] = upvalues_to_string(instr, constants);
    oss << "        upvalues: " + string;
    return {oss.str(), size};
  }
  return {oss.str(), instr.size};
}

inline std::string to_string(const Chunk& chunk, const std::string& name,
                             int level = 0) noexcept {
  const auto& code = chunk.get_code();
  const auto& lines = chunk.get_lines();
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
      auto [string, size] = to_string(instr, pos, chunk.get_constants());
      result += string;
      pos += size;
    });
    result += "\n";
  }
  return result;
}

}  // namespace lox

#endif

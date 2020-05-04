#include "chunk.h"

#include <iomanip>
#include <string>
#include <utility>

#include "object.h"

namespace lox {

static std::pair<std::string, size_t> upvalues_to_string(
    const instruction::Closure& closure,
    const Value_vector& constants) noexcept {
  const auto code = closure.get_code();
  ENSURES(closure.operand() < constants.size());
  const auto value = constants[closure.operand()];
  ENSURES(value.is_object() && value.as_object()->is<Function>());
  const auto func = value.as_object()->template as<Function>();

  std::string result;
  size_t upvalue_pos = closure.size;
  for (size_t i = 0; i < func->upvalue_count; ++i) {
    const auto is_local = code[upvalue_pos++];
    const auto index = code[upvalue_pos++];
    result += std::string{" "} + (is_local ? "local" : "upvalue") + " " +
              std::to_string(index) + ",";
  }
  return {result, upvalue_pos};
}

template <typename Instruction>
static std::pair<std::string, size_t> to_string(
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

std::string to_string(const Chunk& chunk, const std::string& name,
                      int level) noexcept {
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

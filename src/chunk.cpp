#include "chunk.h"

#include <iomanip>
#include <string>
#include <utility>

#include "object.h"

namespace lox {

static std::pair<std::string, size_t> upvalues_to_string(
    const instruction::Closure& closure,
    const Value_vector& constants) noexcept {
  ENSURES(closure.operand() < constants.size());
  const auto value = constants[closure.operand()];
  ENSURES(value.is_object() && value.as_object()->is<Function>());
  const auto* func = value.as_object()->template as<Function>();

  std::string result;
  const auto* code = closure.get_code();
  size_t upvalue_pos = closure.size;
  for (size_t i = 0; i < func->upvalue_count; ++i) {
    const auto is_local = code[upvalue_pos++];
    const auto index = code[upvalue_pos++];
    result += std::string{" "} + (is_local ? "local" : "upvalue") + " " +
              std::to_string(index) + ",";
  }
  return {result, upvalue_pos};
}

static std::string operand_to_string(const instruction::Simple_instr&, size_t,
                                     const Value_vector&) noexcept {
  return "";
}

static std::string operand_to_string(const instruction::Byte_instr& instr,
                                     size_t, const Value_vector&) noexcept {
  std::ostringstream oss;
  oss << " " << static_cast<int>(instr.operand());
  return oss.str();
}

static std::string operand_to_string(const instruction::Constant_instr& instr,
                                     size_t,
                                     const Value_vector& constants) noexcept {
  ENSURES(instr.operand() < constants.size());
  return " " + to_string(constants[instr.operand()], true);
}

static std::string operand_to_string(const instruction::Jump& instr, size_t pos,
                                     const Value_vector&) noexcept {
  std::ostringstream oss;
  oss << " " << instr.operand() << " -> "
      << pos + instruction::Jump::size + instr.operand();
  return oss.str();
}

static std::string operand_to_string(const instruction::Jump_if_false& instr,
                                     size_t pos, const Value_vector&) noexcept {
  std::ostringstream oss;
  oss << " " << instr.operand() << " -> "
      << pos + instruction::Jump_if_false::size + instr.operand();
  return oss.str();
}

static std::string operand_to_string(const instruction::Loop& instr, size_t pos,
                                     const Value_vector&) noexcept {
  std::ostringstream oss;
  oss << " " << instr.operand() << " -> "
      << pos + instruction::Loop::size - instr.operand();
  return oss.str();
}

template <typename Instruction>
static std::pair<std::string, size_t> to_string(
    const Instruction& instr, size_t pos,
    const Value_vector& constants) noexcept {
  std::ostringstream oss;
  oss << instr.name << operand_to_string(instr, pos, constants);
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

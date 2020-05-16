#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <vector>

#include "contract.h"
#include "instruction.h"
#include "value.h"

namespace lox {

struct Chunk {
  template <typename Instruction>
  size_t add(size_t line) noexcept {
    const auto pos = code.size();
    code.push_back(Instruction::opcode);
    lines.push_back(line);
    EXPECTS(code.size() == lines.size());
    return pos;
  }

  template <typename Instruction>
  size_t add(typename Instruction::Operand_t operand, size_t line) noexcept {
    static_assert(Instruction::size > instruction::Simple_instr::size);

    const auto pos = add<Instruction>(line);
    const auto count = Instruction::add_operand(code, operand);
    for (size_t i = 0; i < count; ++i) {
      lines.push_back(line);
    }
    EXPECTS(code.size() == lines.size());
    return pos;
  }

  template <typename Instruction>
  size_t add(size_t operand,
             const typename Instruction::Upvalue_vector &upvalues,
             size_t line) noexcept {
    static_assert(std::is_base_of_v<instruction::Closure_instr, Instruction>);

    const auto pos = add<Instruction>(operand, line);
    const auto count = Instruction::add_upvalues(code, upvalues);
    for (size_t i = 0; i < count; ++i) {
      lines.push_back(line);
    }
    EXPECTS(code.size() == lines.size());
    return pos;
  }

  template <typename... Args>
  size_t add_constant(Args &&... args) noexcept {
    constants.emplace_back(std::forward<Args>(args)...);
    return constants.size() - 1;
  }

  void patch_jump(size_t pos, instruction::Jump::Operand_t operand) noexcept {
    ENSURES(pos < code.size());
    instruction::Jump_instr::set_operand(&code[pos], operand);
  }

  size_t code_size() const noexcept { return code.size(); }
  const Bytecode *code_begin() const noexcept { return &code[0]; }
  const Bytecode *code_end() const noexcept { return &code[code.size()]; }

  const Value_vector &get_constants() const noexcept { return constants; }
  Value_vector &get_constants() noexcept { return constants; }

  size_t line_at(size_t pos) const noexcept {
    ENSURES(pos < lines.size());
    return lines[pos];
  }

  std::string to_string(const std::string &name, int level = 0) const noexcept;

 private:
  using Line_vector = std::vector<size_t>;

  Bytecode_vector code;
  Value_vector constants;
  Line_vector lines;
};

}  // namespace lox

#endif

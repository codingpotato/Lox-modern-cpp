#include "chunk.h"

#include <string>
#include <utility>

#include "contract.h"
#include "instruction.h"
#include "object.h"
#include "value.h"

namespace lox {

std::pair<std::string, size_t> upvalues_to_string(
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

}  // namespace lox

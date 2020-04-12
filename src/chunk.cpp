#include "chunk.h"

#include <string>

#include "object.h"

namespace lox {

std::string upvalues_to_string(const Chunk& chunk, size_t pos,
                               const instruction::Closure& closure) noexcept {
  std::string result;
  auto start = pos + instruction::Closure::size;
  auto value = chunk.constants()[closure.operand()];
  auto func = value.as_object()->template as<Function>();
  for (size_t i = 0; i < func->upvalue_count; ++i) {
    auto is_local = chunk.code()[start++];
    auto index = chunk.code()[start++];
    result += std::string{" "} + (is_local ? "local" : "upvalue") + " " +
              std::to_string(index) + ",";
  }
  return result;
}

}  // namespace lox

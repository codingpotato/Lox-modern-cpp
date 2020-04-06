#include "value.h"

#include "object.h"

namespace lox {

std::string to_string(Value value, bool verbose) noexcept {
  if (value.is_bool()) {
    return value.as_bool() ? "true" : "false";
  }
  if (value.is_double()) {
    return std::to_string(value.as_double());
  }
  if (value.is_object()) {
    return value.as_object()->to_string(verbose);
  }
  return "nil";
}

}  // namespace lox

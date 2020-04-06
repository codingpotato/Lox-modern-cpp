#include "value.h"

#include "object.h"

namespace lox {

std::string to_string(value v, bool verbose) noexcept {
  if (v.is_bool()) {
    return v.as_bool() ? "true" : "false";
  }
  if (v.is_double()) {
    return std::to_string(v.as_double());
  }
  if (v.is_object()) {
    return v.as_object()->to_string(verbose);
  }
  return "nil";
}

}  // namespace lox

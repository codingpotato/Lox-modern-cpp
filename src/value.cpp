#include "value.h"

#include "object.h"

namespace lox {

namespace optimized {

std::string value::repr() const {
  if (is_nil()) {
    return "nil";
  }
  if (is_bool()) {
    return as<bool>() ? "true" : "false";
  }
  if (is_number()) {
    return std::to_string(as<double>());
  }
  if (is_object()) {
    return as<object*>()->repr();
  }
  throw internal_error{""};
}

}  // namespace optimized

namespace tagged_union {

std::string value::repr() const {
  if (is<nil>()) {
    return "nil";
  }
  if (is<bool>()) {
    return as<bool>() ? "true" : "false";
  }
  if (is<double>()) {
    return std::to_string(as<double>());
  }
  if (is<object*>()) {
    return as<object*>()->repr();
  }
  throw internal_error{""};
}

}  // namespace tagged_union

}  // namespace lox

#include "vm.h"

namespace lox {

bool VM::concat_string(Value left, Value right) noexcept {
  if (left.is_object() && right.is_object()) {
    auto obj_left = left.as_object();
    auto obj_right = right.as_object();
    if (obj_left->is<String>() && obj_right->is<String>()) {
      stack.push(heap.make_string(obj_left->as<String>()->get_string() +
                                  obj_right->as<String>()->get_string()));
      return true;
    }
  }
  return false;
}

void VM::throw_runtime_error(const std::string& message) {
  throw Runtime_error{message};
}

void VM::throw_undefined_variable(const String* name) {
  throw Runtime_error{"Undefined variable: '" + name->get_string() + "'."};
}

void VM::throw_incorrect_argument_count(int arity, int argument_count) {
  throw Runtime_error{"Expected " + std::to_string(arity) +
                      " arguments but got " + std::to_string(argument_count) +
                      "."};
}

}  // namespace lox

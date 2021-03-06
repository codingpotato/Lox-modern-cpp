#include "vm.h"

#include <iomanip>

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

void VM::throw_runtime_error(const char* message) {
  throw Runtime_error{message};
}

void VM::throw_undefined_variable(const String* name) {
  throw Runtime_error{"Undefined variable '" + name->get_string() + "'."};
}

void VM::throw_incorrect_argument_count(int arity, int argument_count) {
  throw Runtime_error{"Expected " + std::to_string(arity) +
                      " arguments but got " + std::to_string(argument_count) +
                      "."};
}

void VM::backtrace() const noexcept {
  for (size_t distance = 0; distance < call_frames.size(); ++distance) {
    auto& frame = call_frames.peek(distance);
    auto func = frame.closure->get_func();
    const auto index = frame.ip - func->get_chunk().code_begin();
    *out << "[line " << std::setfill('0') << std::setw(4)
         << func->get_chunk().line_at(index) << "] in " << func->to_string()
         << "\n";
  }
}

}  // namespace lox

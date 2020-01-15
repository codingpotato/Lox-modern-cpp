#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "expression.h"
#include "types.h"

namespace lox {

class interpreter {
 public:
  value evaluate(const expression& expr) {
    return expr.storage.visit(
        overloaded{[this, &expr](const auto& e) { return evaluate(e); }});
  }
};

}  // namespace lox

#endif

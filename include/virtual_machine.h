#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <stack>

#include "statement.h"

namespace lox {

struct virtual_machine {
  struct instruction_pointer {
    statement_id current;
    statement_id last;
  };

  instruction_pointer ip;
  std::stack<instruction_pointer> stack_frame;
};

}  // namespace lox

#endif

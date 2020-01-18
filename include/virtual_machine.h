#ifndef LOX_VIRTUAL_MACHINE_H
#define LOX_VIRTUAL_MACHINE_H

#include <stack>

#include "statement.h"

namespace lox {

struct virtual_machine {
  statement_id ip;
};

}  // namespace lox

#endif

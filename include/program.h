#ifndef LOX_PROGRAM_H
#define LOX_PROGRAM_H

#include <vector>

#include "expression.h"
#include "statement.h"

namespace lox {

using expresion_index = int;
using expression_vector = std::vector<expression>;

struct program {
  expression_vector expressions;
  statement_vector statements;
};

} // namespace lox

#endif
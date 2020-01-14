#ifndef LOX_PROGRAM_H
#define LOX_PROGRAM_H

#include <string>

#include "cache.h"

namespace lox {

struct expression;
struct statement;

using double_cache = cache<double>;
using double_id = double_cache::index;

using string_cache = cache<string>;
using string_id = string_cache::index;

using expression_cache = cache<expression>;
using expression_id = expression_cache::index;

using parameter_cache = cache<expression_id>;
using parameter_id = parameter_cache::index;

using statement_cache = cache<statement>;
using statement_id = statement_cache::index;

struct program {
  double_cache double_literals;
  string_cache string_literals;
  expression_cache expressions;
  parameter_cache parameters;
  statement_cache statements;
  statement_id start_block;
};

}  // namespace lox

#endif
#ifndef LOX_STATEMENT_H
#define LOX_STATEMENT_H

#include <variant>
#include <vector>

#include "expression.h"
#include "program.h"
#include "types.h"
#include "variant_storage.h"

namespace lox {

struct statement {
  struct block {
    block(statement_id f, statement_id l) noexcept : first{f}, last{l} {}

    statement_id first;
    statement_id last;
  };

  struct expression_s {
    explicit expression_s(expression_id e) noexcept : expr{e} {}

    expression_id expr;
  };

  struct function {
    function(string_id n, string_id first, string_id last,
             statement_id b) noexcept
        : name{n}, first_parameter{first}, last_parameter{last}, body{b} {}

    string_id name;
    string_id first_parameter;
    string_id last_parameter;
    statement_id body;
  };

  struct if_else {
    explicit if_else(expression_id c, statement_id then_b,
                     statement_id else_b) noexcept
        : condition{c}, then_block{then_b}, else_block{else_b} {}

    expression_id condition;
    statement_id then_block;
    statement_id else_block;
  };

  struct return_s {
    explicit return_s(expression_id v) noexcept : value{v} {}

    expression_id value;
  };

  struct variable_s {
    variable_s(string_id n, expression_id i) noexcept
        : name{n}, initializer{i} {}

    string_id name;
    expression_id initializer;
  };

  // layout in statements: [block, while]
  struct while_s {
    explicit while_s(expression_id c, statement_id b) noexcept
        : condition{c}, body{b} {}

    expression_id condition;
    statement_id body;
  };

  using element_t = std::variant<block, expression_s, function, if_else,
                                 return_s, variable_s, while_s>;

  template <typename... Args>
  constexpr statement(Args &&... args) noexcept
      : storage{std::forward<Args>(args)...} {}

  variant_storage<element_t> storage;
};

using statement_vector = std::vector<statement>;

}  // namespace lox

#endif
#ifndef LOX_STATEMENT_H
#define LOX_STATEMENT_H

#include <variant>
#include <vector>

#include "expression.h"
#include "program.h"
#include "types.h"

namespace lox {

struct statement {
  struct block {
    statement_id first;
    statement_id last;
  };

  struct expression_s {
    explicit expression_s(expression_id e) noexcept : expr{e} {}

    expression_id expr;
  };

  // layout in statements: [initializer?, body, for]
  struct for_s {
    for_s(bool h_i, expression_id c, expression_id i) noexcept
        : has_initializer{h_i}, condition{c}, increament{i} {}

    bool has_initializer;
    expression_id condition;
    expression_id increament;
  };

  // layout in statements: [body, function]
  struct function {
    function(string_id n, string_id first, string_id last) noexcept
        : name{n}, first_parameter{first}, last_parameter{last} {}

    string_id name;
    string_id first_parameter;
    string_id last_parameter;
  };

  // layout in statements: [then block, else block, if_else]
  struct if_else {
    explicit if_else(expression_id c) noexcept : condition{c} {}

    expression_id condition;
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
    explicit while_s(expression_id c) noexcept : condition{c} {}

    expression_id condition;
  };

  template <typename T, typename... Args>
  constexpr statement(std::in_place_type_t<T> t, Args &&... args) noexcept
      : element{t, std::forward<Args>(args)...} {}

  template <typename T>
  constexpr bool is_type() const noexcept {
    return std::holds_alternative<T>(element);
  }

  template <typename T>
  const auto &get() const noexcept {
    return std::get<T>(element);
  }

  template <typename... Ts>
  auto visit(overloaded<Ts...> &&visitor) const {
    return std::visit(visitor, element);
  }

  using element_t = std::variant<block, expression_s, for_s, function, if_else,
                                 return_s, variable_s, while_s>;

  element_t element;
};

using statement_vector = std::vector<statement>;

}  // namespace lox

#endif
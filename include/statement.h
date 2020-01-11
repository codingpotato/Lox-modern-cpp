#ifndef LOX_STATEMENT_H
#define LOX_STATEMENT_H

#include <string>
#include <variant>
#include <vector>

#include "expression.h"
#include "types.h"

namespace lox {

struct statement {
  struct block {
    block(index_t f, index_t l) noexcept : first{f}, last{l} {}

    index_t first;
    index_t last;
  };

  struct expression_s {
    explicit expression_s(expression e) noexcept : expr{e} {}

    expression expr;
  };

  struct function {
    function(string_id n, std::vector<string_id> ps, block b) noexcept
        : name{n}, parameters{std::move(ps)}, body{b} {}

    string_id name;
    std::vector<string_id> parameters;
    block body;
  };

  struct if_else {
    if_else(expression c, block t, block e) noexcept
        : condition{c}, then_block{t}, else_block{e} {}

    expression condition;
    block then_block;
    block else_block;
  };

  struct return_s {
    explicit return_s(expression v) noexcept : value{v} {}

    expression value;
  };

  struct variable_s {
    variable_s(string_id n, expression i) noexcept : name{n}, initializer{i} {}

    string_id name;
    expression initializer;
  };

  struct while_s {
    expression condition;
    block body;
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

  using element_t = std::variant<block, expression_s, function, if_else,
                                 return_s, variable_s, while_s>;

  element_t element;
};

using statement_vector = std::vector<statement>;

}  // namespace lox

#endif
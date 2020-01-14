#ifndef LOX_EXPRESSION_H
#define LOX_EXPRESSION_H

#include <unordered_map>
#include <variant>

#include "exception.h"
#include "program.h"
#include "token.h"
#include "types.h"

namespace lox {

struct expression {
  enum class operator_t {
    plus,
    minus,
    multiple,
    divide,
    equal,
    not_equal,
    great,
    less,
    great_equal,
    less_equal,
    logic_or,
    logic_and
  };

  struct assignment {
    assignment(string_id var, expression_id v) noexcept
        : variable{var}, value{v} {}

    string_id variable;
    expression_id value;
  };

  struct binary {
    binary(expression_id l, operator_t op, expression_id r) noexcept
        : left{l}, oper{op}, right{r} {}

    expression_id left;
    operator_t oper;
    expression_id right;
  };

  struct call {
    call(expression_id c, parameter_id first, parameter_id last) noexcept
        : callee{c}, first_parameter{first}, last_parameter{last} {}

    expression_id callee;
    parameter_id first_parameter;
    parameter_id last_parameter;
  };

  struct group {
    explicit group(expression_id e) noexcept : expr{e} {}

    expression_id expr;
  };

  struct literal {
    explicit literal() noexcept : value{false} {}
    literal(int i) noexcept : value{i} {}
    literal(double_id d) noexcept : value{d} {}
    literal(string_id id) noexcept : value{id} {}

    std::variant<int, double_id, string_id> value;
  };

  struct unary {
    unary(operator_t op, expression_id e) noexcept : oper{op}, expr{e} {}

    operator_t oper;
    expression_id expr;
  };

  struct variable {
    variable(string_id n) noexcept : name{n} {}

    string_id name;
  };

  using element_t =
      std::variant<assignment, binary, call, group, literal, unary, variable>;

  static operator_t from_token_type(token::type_t type) {
    static const std::unordered_map<token::type_t, operator_t> token_map = {
        {token::minus, operator_t::minus},
        {token::plus, operator_t::plus},
        {token::slash, operator_t::divide},
        {token::star, operator_t::multiple},
        {token::equal_equal, operator_t::equal},
        {token::bang_equal, operator_t::not_equal},
        {token::greater, operator_t::great},
        {token::greater_equal, operator_t::great_equal},
        {token::less, operator_t::less},
        {token::less_equal, operator_t::less_equal},
        {token::k_or, operator_t::logic_or},
        {token::k_and, operator_t::logic_and}};
    if (token_map.find(type) == token_map.end()) {
      throw internal_error("Unknow token type.");
    }
    return token_map.at(type);
  }

  template <typename T, typename... Args>
  constexpr expression(expression_id f, std::in_place_type_t<T> t,
                       Args &&... args) noexcept
      : first{f}, element{t, std::forward<Args>(args)...} {}

  template <typename T>
  constexpr bool is_type() const noexcept {
    return std::holds_alternative<T>(element);
  }

  template <typename T>
  const auto &get() const noexcept {
    return std::get<T>(element);
  }

  template <typename... Ts>
  auto visit(overloaded<Ts...> visitor) const {
    return std::visit(visitor, element);
  }

  expression_id first;
  element_t element;
};

}  // namespace lox

#endif
#ifndef LOX_EXPRESSION_H
#define LOX_EXPRESSION_H

#include <unordered_map>

#include "exception.h"
#include "program.h"
#include "resolver.h"
#include "token.h"
#include "types.h"
#include "variant.h"

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
    assignment(expression_id var, expression_id v) noexcept
        : variable{var}, value{v} {}

    expression_id variable;
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
    using storage_t = variant<null, bool, double_id, string_id>;

    literal() noexcept : storage{null{}} {}
    literal(bool b) noexcept : storage{b} {}
    literal(double_id d) noexcept : storage{d} {}
    literal(string_id id) noexcept : storage{id} {}

    storage_t storage;
  };

  struct unary {
    unary(operator_t op, expression_id e) noexcept : oper{op}, expr{e} {}

    operator_t oper;
    expression_id expr;
  };

  struct variable {
    variable(resolve_info i) noexcept : info{i} {}

    resolve_info info;
  };

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

  template <typename... Args>
  explicit constexpr expression(Args &&... args) noexcept
      : storage{std::forward<Args>(args)...} {}

  using storage_t =
      variant<assignment, binary, call, group, literal, unary, variable>;

  storage_t storage;
};

}  // namespace lox

#endif

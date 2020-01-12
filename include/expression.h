#ifndef LOX_EXPRESSION_H
#define LOX_EXPRESSION_H

#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

#include "exception.h"
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

  struct base {
    index_t first;
  };

  // layout in expressions: [value, assignment]
  struct assignment : base {
    assignment(index_t f, string_id v) noexcept : base{f}, variable{v} {}

    string_id variable;
  };

  // layout in expressions: [left, right, binary]
  struct binary : base {
    binary(index_t f, operator_t op) noexcept : base{f}, oper{op} {}

    operator_t oper;
  };

  // layout in expressions: [callee, argument*, call]
  struct call : base {
    explicit call(index_t f) noexcept : base{f} {}
  };

  struct group : base {
    explicit group(index_t f) noexcept : base{f} {}
  };

  struct literal : base {
    explicit literal(index_t f) noexcept : base{f}, value{null{}} {}
    literal(index_t f, bool b) noexcept : base{f}, value{b} {}
    literal(index_t f, int i) noexcept : base{f}, value{i} {}
    // literal(index_t f, double d) noexcept : base{f}, value{d} {}
    literal(index_t f, string_id id) noexcept : base{f}, value{id} {}

    std::variant<null, bool, int, string_id> value;
  };

  struct unary : base {
    unary(index_t f, operator_t op) noexcept : base{f}, oper{op} {}

    operator_t oper;
  };

  struct variable : base {
    variable(index_t f, string_id n) noexcept : base{f}, name{n} {}

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
  constexpr expression(std::in_place_type_t<T> t, Args &&... args) noexcept
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
  auto visit(overloaded<Ts...> visitor) const {
    return std::visit(visitor, element);
  }

  element_t element;
};

using expression_vector = std::vector<expression>;

}  // namespace lox

#endif
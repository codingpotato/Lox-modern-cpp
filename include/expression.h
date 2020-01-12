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

  // layout in expressions: [value, assignment]
  struct assignment {
    explicit assignment(string_id v) noexcept : variable{v} {}

    string_id variable;
  };

  // layout in expressions: [left, right, binary]
  struct binary {
    explicit binary(operator_t op) noexcept : oper{op} {}

    operator_t oper;
  };

  // layout in expressions: [callee, argument*, call]
  struct call {};

  struct group {};

  struct literal {
    literal() noexcept : value{null{}} {}
    explicit literal(bool b) noexcept : value{b} {}
    explicit literal(int i) noexcept : value{i} {}
    explicit literal(double d) noexcept : value{d} {}
    explicit literal(string_id id) noexcept : value{id} {}

    std::variant<null, bool, int, double, string_id> value;
  };

  struct unary {
    explicit unary(operator_t op) noexcept : oper{op} {}

    operator_t oper;
  };

  struct variable {
    explicit variable(string_id n) noexcept : name{n} {}

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
  constexpr expression(index_t f, std::in_place_type_t<T> t,
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

  index_t first;
  element_t element;
};

using expression_vector = std::vector<expression>;

}  // namespace lox

#endif
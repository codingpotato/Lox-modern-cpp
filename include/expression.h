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

struct expression {
  struct binary {
    binary(index_t l, operator_t op, index_t r) noexcept
        : left{l}, oper{op}, right{r} {}

    index_t left;
    operator_t oper;
    index_t right;
  };

  struct assignment {
    assignment(string_id var, index_t v) noexcept : variable{var}, value{v} {}

    string_id variable;
    index_t value;
  };

  struct call {
    call(index_t c, index_vector args) noexcept
        : callee{c},
          arguments{std::make_unique<index_vector>(std::move(args))} {}

    index_t callee;
    std::unique_ptr<index_vector> arguments;
  };

  struct group {
    explicit group(index_t e) noexcept : expr{e} {}

    index_t expr;
  };

  struct literal {
    struct null {};

    literal() noexcept : value{null{}} {}
    explicit literal(bool b) noexcept : value{b} {}
    explicit literal(int i) noexcept : value{i} {}
    explicit literal(double d) noexcept : value{d} {}
    explicit literal(string_id id) noexcept : value{id} {}

    std::variant<null, bool, int, double, string_id> value;
  };

  struct unary {
    unary(operator_t o, index_t e) noexcept : oper{o}, expr{e} {}

    operator_t oper;
    index_t expr;
  };

  struct variable {
    variable(string_id n) noexcept : name{n} {}

    string_id name;
  };

  using element =
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
      throw internal_error("Unknow token");
    }
    return token_map.at(type);
  }

  index_t first;
  index_t last;
};

using expression_vector = std::vector<expression::element>;

}  // namespace lox

#endif
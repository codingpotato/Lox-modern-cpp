#ifndef LOX_EXPRESSION_H
#define LOX_EXPRESSION_H

#include <unordered_map>
#include <variant>

#include "exception.h"
#include "token.h"
#include "types.h"

namespace lox {

struct expression {
  enum operator_t {
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

  struct binary {
    binary(index_t l, operator_t op, index_t r) noexcept
        : left{l}, oper{op}, right{r} {}

    index_t left;
    operator_t oper;
    index_t right;
  };

  struct assignment {
    std::string name;
    index_t value;
  };

  struct call {
    index_t callee;
    std::vector<index_t> arguments;
  };

  struct group {
    index_t expr;
  };

  // struct literal {
  //  value v_;
  //};

  struct unary {
    operator_t oper;
    index_t expr;
  };

  struct variable {
    std::string name_;
  };

  using element =
      std::variant<assignment, binary, call, group, unary, variable>;

  static operator_t from_token_type(token::type_t type) {
    static const std::unordered_map<token::type_t, expression::operator_t>
        token_map = {{token::minus, expression::minus},
                     {token::plus, expression::plus},
                     {token::slash, expression::divide},
                     {token::star, expression::multiple},
                     {token::equal_equal, expression::equal},
                     {token::bang_equal, expression::not_equal},
                     {token::greater, expression::great},
                     {token::greater_equal, expression::great_equal},
                     {token::less, expression::less},
                     {token::less_equal, expression::less_equal},
                     {token::k_or, expression::logic_or},
                     {token::k_and, expression::logic_and}};
    if (token_map.find(type) == token_map.end()) {
      throw parse_error("");
    }
    return token_map.at(type);
  }

  index_t first;
  index_t last;
};

using expression_vector = std::vector<expression::element>;

} // namespace lox

#endif
#ifndef LOX_EXPRESSION_H
#define LOX_EXPRESSION_H

#include <variant>

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
    int left;
    operator_t oper;
    int right;
  };

  struct node {
    std::variant<binary> expr;
  };
};

}  // namespace lox

#endif
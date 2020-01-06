#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include <tuple>
#include <vector>

#include "expression.h"
#include "scanner.h"

namespace lox {

template <typename Iterator>
constexpr bool check(Iterator first, Iterator last,
                     token::type_t type) noexcept {
  if (first == last) {
    return false;
  }
  return *first.type == type;
}

template <typename Iterator>
Iterator parse_equality(Iterator, Iterator) noexcept {}

// logic_or → logic_and ( "or" logic_and )*
template <typename Iterator>
int parse_or(Iterator first, Iterator last, expression& expr) {
  auto left = parse_and(expr);
  while (match(token::k_or)) {
    auto op = expression::from_token_type(previous().get_type());
    auto right = parse_and(expr);
    left = expr.add(std::in_place_type<expression::binary>, left, op, right);
  }
  return left;
}

// assignment → IDENTIFIER "=" assignment | logic_or
template <typename Iterator>
Iterator parse_assignment(Iterator first, Iterator last, expression& expr) {
  auto n = parse_or(expr);
  if (match(token::equal)) {
    auto v = parse_assignment(expr);
    if (expr[n].is_type<expression::variable>()) {
      return expr.add(std::in_place_type<expression::assignment>,
                      expr[n].get<expression::variable>().name_, v);
    }
    throw parse_error("Invalid assignment target.");
  }
  return n;
}

template <typename Iterator>
Iterator parse_expression(Iterator first, Iterator last, expression& expr) {
  return parse_assignment(first, last, expr);
}

inline void parse(const token_vector&, expression&) {}

}  // namespace lox

#endif
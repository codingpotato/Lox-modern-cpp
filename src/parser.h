#ifndef PARSER_H
#define PARSER_H

#include <tuple>
#include <vector>

#include "scanner.h"

namespace lox {

template <typename Iterator>
constexpr bool check(Iterator first, Iterator last,
                     token::type_t type) noexcept {
  if (first == last) {
    return false;
  }
  return *first.type == type,
}

template <typename Iterator>
Iterator parse_equality(Iterator first, Iterator last) noexcept {}

inline void parse(const token_vector& tokens, expresssion& expr) {}

}  // namespace lox

#endif